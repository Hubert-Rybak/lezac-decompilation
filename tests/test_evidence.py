"""Contracts for the complete, real REA export (no original game required)."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / "analysis" / "rea" / "verified"
SHA256 = "7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec"


def read(name):
    return json.loads((EVIDENCE / name).read_text())


def test_complete_export_has_matching_identities():
    summary = read("summary.json")
    inventory = read("procedures.json")["normalized_result"]
    overview = read("overview.json")["normalized_result"]
    assert summary["discovered"] == summary["exported"] == overview["procedure_count"] == 215
    assert not summary["failures"]
    assert summary["original_unchanged"] is True
    addresses = {p["address"] for p in inventory}
    assert len(addresses) == len(inventory) == 215
    assert addresses == {p["address"] for p in summary["functions"]}
    assert {p.stem for p in (EVIDENCE / "functions").glob("*.json")} == {a[2:] for a in addresses}
    for row in summary["functions"]:
        stem = row["address"][2:]
        evidence = read(f"functions/{stem}.json")
        dossier = evidence["normalized_result"]
        assert evidence["subject"]["digest"]["sha256"] == SHA256
        assert evidence["provider"]["id"] == "ghidra"
        assert evidence["provider"]["version"] == "12.1.4"
        assert evidence["evidence_id"] == row["evidence_id"]
        assert dossier["procedure"]["address"] == row["address"]
        assert dossier["assembly"]
        assert dossier["pseudocode"].strip()
        source = (EVIDENCE / "pseudocode" / f"{stem}.c").read_text()
        assembly = (EVIDENCE / "assembly" / f"{stem}.asm").read_text()
        assert source.endswith(dossier["pseudocode"] + "\n")
        assert assembly.endswith("\n".join(dossier["assembly"]) + "\n")
        assert "not compilable reconstructed source" in source
        body = dossier["procedure"]["body"]
        assert body["available"] and body["contains_entry"]
        assert sum(int(r["end"], 16) - int(r["start"], 16) + 1 for r in body["ranges"]) == body["total_bytes"]


def test_loaded_image_is_verified():
    image = read("load-image.json")["normalized_result"]
    assert image["status"] == "verified"
    assert image["target_sha256"] == SHA256
    assert len(image["checks"]) == 94
    assert all(check["matched"] for check in image["checks"])
    assert image["header_bytes"] == 1904
    assert image["module_bytes"] == 50480
    assert image["overlay_bytes"] == 0
    assert read("close.json")["result"] is None


def test_oversized_actor_dossier_was_recovered_without_truncation():
    record = read("transport-16053.json")
    evidence = read("functions/16053.json")
    assert record["error"]["details"]["resource"] == "transport"
    assert record["recovered_evidence_id"] == evidence["evidence_id"]
    assert evidence["normalized_result"]["procedure"]["body"]["total_bytes"] == 5934
