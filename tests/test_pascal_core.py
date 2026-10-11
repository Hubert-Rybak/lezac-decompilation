"""Free Pascal TP-mode reconstruction versus the ORIGINAL relocated DOS code.

The cases below call the existing oracle tests, never their C-build fixture.
FPC/FPC_UNITS select the native compiler/RTL; FPC8086/FPC8086_UNITS select
an optional i8086-msdos compile check. All generated files live under build/.
"""
import ctypes as C

from pathlib import Path
import shlex
import subprocess
import tempfile

import pytest

import test_recovered as oracle
from dos_machine import ROOT

CORE = ROOT / "pascal/LZCORE.PAS"
BRIDGE = ROOT / "pascal/LZBRIDGE.PAS"
BUILD = ROOT / "build/pascal-tests"
CASES = (
    ("test_checksum_differential", 0x10000, 293),
    ("test_animation_init_differential", 0x106AB, 1137),
    ("test_wrapping_add_clamp_differential", 0x15715, 96512),
    ("test_friction_all_int16_values", 0x15B86, 65536),
    ("test_state45_differential", 0x12E8D, 519),
    ("test_tile_collection_differential", 0x15AFD, 9728),
)

# This fixture loads/checks the original executable and executes its real guard.
machine = oracle.machine


def compiler_command(variable, default, units_variable):
    from test_pascal_build import module
    compiler, flags = module().toolchain('native' if variable == 'FPC' else 'dos')
    return [compiler, *flags, '-Mtp', '-B', '-O2']


def compile_pascal(command):
    print("\nPascal compile:", shlex.join(map(str, command)))
    result = subprocess.run(command, cwd=BUILD, text=True, capture_output=True)
    print(result.stdout, end="")
    print(result.stderr, end="")
    assert result.returncode == 0, "Free Pascal compilation failed"


@pytest.fixture(scope="module")
def native():
    if not CORE.is_file() or not BRIDGE.is_file():
        yield None  # Missing implementation must be a test failure, not an error.
        return
    BUILD.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="host-", dir=BUILD) as directory:
        output = Path(directory) / "liblezac_pascal.so"
        command = compiler_command("FPC", "fpc", "FPC_UNITS")
        command += ["-Cg", "-Fu" + str(CORE.parent), "-FU" + directory,
                    "-FE" + directory, "-o" + str(output), str(BRIDGE)]
        compile_pascal(command)
        assert output.is_file()
        library = C.CDLL(str(output))
        signatures = {
            "lezac_alternating_checksum": ([oracle.U16, oracle.P8, oracle.U16], oracle.U16),
            "lezac_animation_init": ([oracle.P8, oracle.U8, oracle.U8, oracle.U8, oracle.U8], None),
            "lezac_wrapping_add_clamp": ([oracle.U8, oracle.U8, oracle.U8], oracle.U8),
            "lezac_friction_step": ([oracle.I16], oracle.I16),
            "lezac_state45_init": ([oracle.P8, oracle.U16, oracle.P16], None),
            "lezac_collect_tile": ([oracle.P8, oracle.P16, oracle.U16, oracle.P16,
                                    oracle.P16, C.POINTER(oracle.Collection)], None),
        }
        for name, (arguments, result) in signatures.items():
            function = getattr(library, name)
            function.argtypes = arguments
            function.restype = result
        yield library


def test_compiler_configuration_matches_shared_build_driver(monkeypatch):
    from test_pascal_build import module
    for key in ('FPC', 'FPC_UNITS', 'FPC_RTL'):
        monkeypatch.delenv(key, raising=False)
    compiler, flags = module().toolchain('native')
    command = compiler_command('FPC', 'fpc', 'FPC_UNITS')
    assert command[:1 + len(flags)] == [compiler, *flags]


def test_original_loader_and_real_stack_check(machine):
    oracle.test_original_loader_and_real_stack_check(machine)


@pytest.mark.parametrize("case,entry,count", CASES, ids=[case[0] for case in CASES])
def test_pascal_original_differential(machine, native, case, entry, count):
    assert native is not None, "Pascal reconstruction is not implemented yet"
    before = machine.calls[entry]
    getattr(oracle, case)(machine, native)
    assert machine.calls[entry] - before == count
    machine.verify_unmodified()
    print(f"\nPascal/original {entry:#x}: {count} differential cases")


def test_pascal_tp_layout(native):
    assert native is not None, "Pascal reconstruction is not implemented yet"
    # Run a separate TP-mode executable, not a C view of Pascal types.
    source_text = """program Layout;
{$MODE TP}
uses LZCORE;
var A: TAnimState; S: TInitState45; C: TCollectionState;
begin
  WriteLn(SizeOf(Integer), ' ', SizeOf(Word), ' ', SizeOf(Byte), ' ',
          SizeOf(TAnimState), ' ', SizeOf(TInitState45), ' ',
          SizeOf(TWordTable12), ' ', SizeOf(TCollectionState));
  WriteLn(PtrUInt(@A.Step) - PtrUInt(@A), ' ',
          PtrUInt(@S.First) - PtrUInt(@S), ' ',
          PtrUInt(@S.Second) - PtrUInt(@S), ' ',
          PtrUInt(@S.Mode) - PtrUInt(@S), ' ',
          PtrUInt(@C.AbovePresent) - PtrUInt(@C));
end.
"""
    with tempfile.TemporaryDirectory(prefix="layout-", dir=BUILD) as directory:
        source = Path(directory) / "LAYOUT.PAS"
        source.write_text(source_text)
        output = Path(directory) / "layout"
        command = compiler_command("FPC", "fpc", "FPC_UNITS")
        command += ["-Fu" + str(CORE.parent), "-FU" + directory,
                    "-FE" + directory, "-o" + str(output), str(source)]
        compile_pascal(command)
        result = subprocess.run([str(output)], check=True, text=True, capture_output=True)
        assert result.stdout.splitlines() == ["2 2 1 7 45 24 6", "6 4 24 44 5"]
        print("\nPascal TP layout:", result.stdout.strip())


def test_core_compiles_for_i8086_tp():
    assert CORE.is_file(), "Pascal reconstruction is not implemented yet"
    from test_pascal_build import module
    builder = module()
    try:
        builder.toolchain('dos')
    except builder.ToolchainUnavailable:
        pytest.skip("Set FPC_DOS and FPC_DOS_RTL for the DOS compile check")
    BUILD.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="i8086-", dir=BUILD) as directory:
        command = compiler_command("FPC8086", "ppcross8086", "FPC8086_UNITS")
        command += ["-Tmsdos", "-WmCompact", "-XX", "-FU" + directory, "-FE" + directory, str(CORE)]
        compile_pascal(command)
        assert list(Path(directory).glob("*.ppu")), "No i8086 compiled unit produced"
        assert list(Path(directory).glob("*.o")), "No i8086 object produced"
