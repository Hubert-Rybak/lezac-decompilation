import test from 'node:test';
import assert from 'node:assert/strict';
import {unwrapResult, checkInventory, safeStem, renderDossier} from '../tools/rea_helpers.mjs';

test('unwrap primitive and evidence responses without losing false/null',()=>{
 assert.deepEqual(unwrapResult({structuredContent:{result:[{address:'0x10000'}]}}),[{address:'0x10000'}]);
 assert.equal(unwrapResult({structuredContent:{result:false}}),false);
 assert.equal(unwrapResult({structuredContent:{result:null}}),null);
 assert.equal(unwrapResult({structuredContent:{normalized_result:'C code'}}),'C code');
 assert.throws(()=>unwrapResult({isError:true,content:[{type:'text',text:'bad'}]}));
 assert.throws(()=>unwrapResult({content:[]}));
});
test('inventory validation rejects duplicate or invalid addresses and count drift',()=>{
 const rows=[{address:'0x10000',name:'entry'},{address:'0x10100',name:'f'}];
 assert.equal(checkInventory(rows,2).length,2);
 assert.throws(()=>checkInventory(rows,3));
 assert.throws(()=>checkInventory([...rows,rows[0]],3));
 assert.throws(()=>checkInventory([{address:'oops'}],1));
 assert.throws(()=>checkInventory(null,0));
});
test('REA assembly is a list of instruction lines, not a string',()=>{
 const e={evidence_id:'ev_test',normalized_result:{procedure:{address:'0x10000',name:'checksum'},pseudocode:'void checksum(void) {}',assembly:['0x10000: PUSH BP','0x10001: RET']}};
 const result=renderDossier(e);
 assert.match(result.assembly,/PUSH BP\n0x10001: RET/);
 assert.match(result.pseudocode,/not compilable/);
 assert.match(result.pseudocode,/void checksum/);
 assert.throws(()=>renderDossier({...e,normalized_result:{...e.normalized_result,assembly:'invalid'}}));
});

test('select retained evidence by exact identity after transport overflow',async()=>{
 const helpers=await import('../tools/rea_helpers.mjs');
 assert.equal(typeof helpers.selectRetained,'function');
 const record={evidence_id:'ev_abc',subject:{digest:{sha256:'expected'}},normalized_result:{procedure:{address:'0x16053'}}};
 assert.equal(helpers.selectRetained({records:[record]},'ev_abc','expected','0x16053'),record);
 assert.throws(()=>helpers.selectRetained({records:[record]},'missing','expected','0x16053'));
 assert.throws(()=>helpers.selectRetained({records:[record]},'ev_abc','wrong','0x16053'));
 assert.throws(()=>helpers.selectRetained({records:[record]},'ev_abc','expected','0x10000'));
 assert.throws(()=>helpers.selectRetained({records:[record,record]},'ev_abc','expected','0x16053'));
});

test('only address-derived safe output filenames',()=>{
 assert.equal(safeStem('0x10000'),'10000');
 assert.throws(()=>safeStem('../../escape'));
});
