import pytest
import shlex
import subprocess

from glob import glob
from pathlib import Path

base = Path(__file__).parent.parent.absolute()

@pytest.fixture(scope="session")
def exe(pytestconfig):
    return pytestconfig.getoption("exe")

def run_cj_echo(exe, filepath):
    return subprocess.run(shlex.split(str(exe)) + ['--csp', filepath], capture_output=True)

filepaths = glob(str(base/'data/**/*.json'), recursive=True)
@pytest.mark.parametrize('filepath', filepaths)
def test_cj_echo(filepath, exe):
    filepath = base / 'data' / filepath
    with open(filepath, 'r') as f:
        csp = f.read()
        r = run_cj_echo(exe, str(filepath))
        assert r.returncode == 0
        assert csp == r.stdout.decode('utf-8')
