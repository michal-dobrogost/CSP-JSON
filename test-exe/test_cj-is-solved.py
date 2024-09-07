import pytest
import shlex
import subprocess

from glob import glob
from pathlib import Path

base = Path(__file__).parent.parent.absolute()
australia_path = base / 'data/human/color-australia.json'

@pytest.fixture(scope="session")
def exe(pytestconfig):
    return pytestconfig.getoption("exe")

def run_cj_is_solved(exe, filepath, solution):
    return subprocess.run(shlex.split(str(exe)) + ['--csp', filepath, '--solution', solution], capture_output=True)

def test_australia_true(exe):
    r = run_cj_is_solved(exe, australia_path, '[0,1,2,0,1,0,0]')
    assert r.returncode == 0
    assert r.stdout.decode('utf-8') == "true\n"

def test_australia_false(exe):
    r = run_cj_is_solved(exe, australia_path, '[0,0,0,0,0,0,0]')
    assert r.returncode == 0
    assert r.stdout.decode('utf-8') == "false\n"

def test_australia_too_short(exe):
    r = run_cj_is_solved(exe, australia_path, '[0,0,0,0,0,0]')
    assert r.returncode != 0
    assert r.stdout.decode('utf-8') == ""

def test_australia_too_long(exe):
    r = run_cj_is_solved(exe, australia_path, '[0,0,0,0,0,0,0,0]')
    assert r.returncode != 0
    assert r.stdout.decode('utf-8') == ""

def test_australia_no_file(exe):
    r = run_cj_is_solved(exe, base/'this-file-does-not-exist.json', '[0,0,0,0,0,0,0]')
    assert r.returncode != 0
    assert r.stdout.decode('utf-8') == ""
