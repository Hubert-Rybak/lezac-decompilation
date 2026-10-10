export function unwrapResult(response) {
  if (response.isError === true) throw new Error(JSON.stringify(response.content));
  const value = response.structuredContent;
  if (value && Object.hasOwn(value,'normalized_result')) return value.normalized_result;
  if (value && Object.hasOwn(value,'result')) return value.result;
  throw new Error('REA response omitted a structured result');
}
export function safeStem(address) {
  if (typeof address !== 'string' || !/^0x[0-9a-f]+$/i.test(address)) throw new Error('Invalid linear address');
  return BigInt(address).toString(16);
}
export function renderDossier(evidence) {
  const d=evidence.normalized_result;
  if(!d?.procedure || typeof d.pseudocode!=='string' || !Array.isArray(d.assembly) || !d.assembly.every(x=>typeof x==='string')) throw new Error('Dossier omitted pseudocode or instruction lines');
  safeStem(d.procedure.address);
  return {
    pseudocode:`/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.\n * ${d.procedure.address} ${d.procedure.name}\n * ${evidence.evidence_id}\n */\n${d.pseudocode}\n`,
    assembly:`; ${d.procedure.address} ${d.procedure.name}\n${d.assembly.join('\n')}\n`
  };
}

export function selectRetained(bundle, evidenceId, sha256, address) {
  const matches=bundle.records?.filter(x=>x.evidence_id===evidenceId);
  if(matches?.length!==1) throw new Error('Missing or duplicate retained evidence');
  const evidence=matches[0];
  if(evidence.subject?.digest?.sha256!==sha256 || evidence.normalized_result?.procedure?.address!==address) throw new Error('Retained evidence identity mismatch');
  return evidence;
}

export function checkInventory(rows, expected) {
  if (!Array.isArray(rows) || rows.length !== expected) throw new Error('Procedure count mismatch');
  const seen = new Set();
  for (const row of rows) {
    const key = safeStem(row.address);
    if (seen.has(key)) throw new Error('Duplicate procedure address');
    seen.add(key);
  }
  return rows;
}
