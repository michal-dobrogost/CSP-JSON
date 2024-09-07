import pytest
import shlex
import subprocess

from glob import glob
from pathlib import Path

base = Path(__file__).parent.parent.absolute()

@pytest.fixture(scope="session")
def exe(pytestconfig):
    return pytestconfig.getoption("exe")

def run_cj_validate(exe, filepath):
    return subprocess.run(shlex.split(str(exe)) + ['--csp', filepath], capture_output=True)

filepaths = glob(str(base/'data/**/*.json'), recursive=True)
@pytest.mark.parametrize('filepath', filepaths)
def test_cj_echo_pos(filepath, exe):
    print(filepath)
    r = run_cj_validate(exe, filepath)
    assert r.returncode == 0
    assert r.stdout.decode('utf-8') == "OK\n"

filepaths = glob(str(base/'test/data/validation-neg/**/*.json'), recursive=True)
@pytest.mark.parametrize('filepath', filepaths)
def test_cj_echo_neg(filepath, exe):
    r = run_cj_validate(exe, filepath)
    assert r.returncode != 0
    assert r.stdout.decode('utf-8') == "Invalid\n"
