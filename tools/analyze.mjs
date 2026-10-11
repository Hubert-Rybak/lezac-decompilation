#!/usr/bin/env node
// Export the complete provider-discovered inventory, not an arbitrary sample.
import {Client} from '@modelcontextprotocol/client';
import {StdioClientTransport} from '@modelcontextprotocol/client/stdio';
import {mkdir,readFile,writeFile,unlink} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve,join,dirname} from 'node:path';
import {createRequire} from 'node:module';
import {fileURLToPath} from 'node:url';
import {unwrapResult,checkInventory,safeStem,renderDossier,selectRetained} from './rea_helpers.mjs';
const require=createRequire(import.meta.url);
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const input=resolve(process.argv[2] || join(root,'original/game/LEZAC.EXE'));
const out=resolve(process.argv[3] || join(root,'analysis/rea/session'));
const expected='7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec';
const hash=async()=>createHash('sha256').update(await readFile(input)).digest('hex');
if (await hash()!==expected) throw new Error('Input differs from the pinned v1.0 executable');
await mkdir(out,{recursive:false}); // New directory required; never overwrite evidence.
await mkdir(join(out,'functions'));
await mkdir(join(out,'pseudocode'));
await mkdir(join(out,'assembly'));
const entry=resolve(dirname(require.resolve('rea-agents/package.json')),'scripts/rea.mjs');
const transport=new StdioClientTransport({command:process.execPath,args:[entry,'mcp'],env:{...process.env},stderr:'pipe'});
let stderr='';
transport.stderr?.on('data',chunk=>{stderr+=chunk.toString();});
const client=new Client({name:'lezac-decompilation',version:'0.1.0'});
let opened=false;
const failures=[];
const exported=[];
async function save(path,data) {await writeFile(join(out,path),JSON.stringify(data,null,2)+'\n',{flag:'wx'});}
async function call(name,args={},file) {
  let response=await client.callTool({name,arguments:args},{timeout:240000});
  if(response.isError && name==='analyze_function') {
    const text=response.content?.find(x=>x.type==='text')?.text;
    const error=text ? JSON.parse(text).error : null;
    const evidenceId=error?.details?.reported_limits?.evidence_id;
    if(error?.code==='resource_constraint' && error?.details?.resource==='transport' && evidenceId) {
      const bundlePath=join(out,`retained-${safeStem(args.procedure)}.json`);
      await call('export_evidence_bundle',{path:bundlePath,overwrite:false});
      const evidence=selectRetained(JSON.parse(await readFile(bundlePath,'utf8')),evidenceId,expected,args.procedure);
      response={structuredContent:evidence};
      // Keep the exact requested record; the bundle duplicates prior records.
      await save(`transport-${safeStem(args.procedure)}.json`,{error,recovered_evidence_id:evidenceId});
      await unlink(bundlePath);
    }
  }
  const value=unwrapResult(response);
  if(file) await save(file,response.structuredContent);
  return {value,evidence:response.structuredContent};
}
try {
  await client.connect(transport);
  const open=await call('open_binary',{path:input,provider_id:'ghidra'},'open.json');
  opened=true;
  if(open.value.sha256!==expected) throw new Error('REA input identity mismatch');
  const overview=await call('binary_overview',{},'overview.json');
  const inventory=await call('list_procedures',{},'procedures.json');
  const procedures=checkInventory(inventory.value,overview.value.procedure_count);
  console.log(`REA discovered ${procedures.length} procedures`);
  const image=await call('inspect_native_load_image',{},'load-image.json');
  if(image.value.status!=='verified') throw new Error(`Load image: ${image.value.status}`);
  await call('list_strings',{},'strings.json');
  for(const [index,procedure] of procedures.entries()) {
    const stem=safeStem(procedure.address);
    try {
      const {value:dossier,evidence}=await call('analyze_function',{procedure:procedure.address},`functions/${stem}.json`);
      const rendered=renderDossier(evidence);
      await writeFile(join(out,'pseudocode',`${stem}.c`),rendered.pseudocode,{flag:'wx'});
      await writeFile(join(out,'assembly',`${stem}.asm`),rendered.assembly,{flag:'wx'});
      exported.push({address:procedure.address,name:dossier.procedure.name,evidence_id:evidence.evidence_id,body:dossier.procedure.body});
    } catch(error) {
      failures.push({address:procedure.address,message:String(error)});
      console.error(`FAILED ${procedure.address}: ${error}`);
    }
    if((index+1)%20===0 || index+1===procedures.length) console.log(`Exported ${index+1}/${procedures.length}; failures=${failures.length}`);
  }
  if(await hash()!==expected) throw new Error('Original input changed during analysis');
  await save('summary.json',{target_sha256:expected,discovered:procedures.length,exported:exported.length,failures,functions:exported,original_unchanged:true});
  if(failures.length) process.exitCode=1;
} finally {
  if(opened) {
    try {await call('close_binary',{},'close.json');} catch(error) {console.error(`Cleanup failed: ${error}`);process.exitCode=1;}
  }
  await client.close();
  await writeFile(join(out,'provider-stderr.log'),stderr,{flag:'wx'});
}
