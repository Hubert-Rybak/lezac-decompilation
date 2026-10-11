"""CI must hash the same pinned requirements file that pip installs."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def test_pip_cache_uses_the_installed_requirements_file():
    workflow = (ROOT / '.github/workflows/verify.yml').read_text()
    setup = re.search(r'uses: actions/setup-python@[^\n]+\n(.*?)(?=\n\s+- (?:uses|name|run):)',
                      workflow, re.S)
    assert setup is not None
    cache = re.search(r'cache-dependency-path:\s*([^\s#]+)', setup[1])
    assert cache is not None, 'setup-python must explicitly locate requirements-dev.txt'
    installed = re.search(r'python -m pip install -r ([^\s]+)', workflow)
    assert installed is not None
    cached_path = cache[1].strip('\"\'')
    assert cached_path == installed[1]
    assert (ROOT / cached_path).is_file()
