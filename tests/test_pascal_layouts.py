"""Compile measured original-layout declarations, both host and 16-bit DOS."""
import importlib.util
from pathlib import Path
import subprocess
import pytest

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = [7,38,4,8,11,15,13,45,16,30,7,14,6,3,8,128]
NAMES = ['TAnimation7','TActor38','TSpriteDef4','TRenderSprite8','TDebris11',
         'TGroup15','TScore13','TScoreDisplay45','TMotion16','TSpawn30',
         'TMapPoint7','TTileReplacement14','TSound6','TRGB3','TLevelHeader8','TFile128']


def load(name):
    spec = importlib.util.spec_from_file_location(name, ROOT / 'tools' / (name + '.py'))
    assert spec and spec.loader
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def source():
    assert (ROOT / 'pascal/LZTYPES.PAS').exists(), 'Original-layout Pascal declarations not implemented'
    declarations = 'program Layout; {$mode tp} uses LZTypes;\nvar A:TActor38; M:TMotion16; S:TScore13; L:TLevelHeader8; F:TFile128;\nbegin\n'
    sizes = '\n'.join('WriteLn(SizeOf(' + name + '));' for name in NAMES)
    offsets = """
WriteLn(Ofs(A.Unknown05)-Ofs(A), ' ', Ofs(A.VelocityX)-Ofs(A), ' ',
        Ofs(A.FractionX)-Ofs(A), ' ', Ofs(A.Animation)-Ofs(A), ' ',
        Ofs(A.SavedAnimation)-Ofs(A), ' ', Ofs(A.Energy)-Ofs(A));
WriteLn(Ofs(M.BaseX)-Ofs(M), ' ', Ofs(M.BaseY)-Ofs(M), ' ',
        Ofs(S.Name)-Ofs(S), ' ', Ofs(L.RequiredCount)-Ofs(L), ' ', Ofs(F.FileName)-Ofs(F));
end.
"""
    return declarations + sizes + offsets


@pytest.mark.parametrize('target', ['native', 'dos'])
def test_compiled_record_sizes_and_packed_offsets(tmp_path, target):
    text = source()
    builder = load('build_pascal')
    try:
        builder.toolchain(target)
    except builder.ToolchainUnavailable:
        if target == 'dos':
            pytest.skip('Optional i8086 cross compiler not installed')
        raise
    program = tmp_path / 'LAYOUT.PAS'
    program.write_text(text)
    output = tmp_path / target / ('LAYOUT.EXE' if target == 'dos' else 'layout')
    builder.build(program, output, target)
    if target == 'dos':
        proof = load('run_dos_probe').run_mz(output.read_bytes())
        assert proof['exit_code'] == 0
        lines = proof['stdout'].splitlines()
    else:
        lines = subprocess.check_output([str(output)], text=True).splitlines()
    assert list(map(int, lines[:len(EXPECTED)])) == EXPECTED
    assert lines[len(EXPECTED):] == ['5 6 10 22 29 36', '7 9 4 5 48']
