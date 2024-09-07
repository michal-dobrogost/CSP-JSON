import pytest
import shlex
import subprocess

from difflib import Differ
from glob import glob
from pathlib import Path

base = Path(__file__).parent.parent.absolute()

@pytest.fixture(scope="session")
def exe(pytestconfig):
    return pytestconfig.getoption("exe")

def run_cj_gen_urbcsp(exe, args):
    return subprocess.run(shlex.split(str(exe)) + args, capture_output=True)

expected = '''{
  "meta": {
    "id": "urbcsp/n100d10c10t10s100i99k10",
    "algo": "urbcsp",
    "params": {"n": 100, "d": 10, "c": 10, "t": 10, "s": 100, "i": 99, "k": 10}
  },
  "domains": [
    {"values": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]}
  ],
  "vars": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  "constraintDefs": [
    {"noGoods": [[2, 0], [2, 7], [3, 8], [4, 8], [5, 6], [5, 7], [6, 6], [8, 2], [9, 6], [9, 9]]},
    {"noGoods": [[0, 1], [0, 3], [2, 2], [3, 0], [7, 6], [8, 0], [8, 4], [8, 8], [9, 1], [9, 2]]},
    {"noGoods": [[1, 1], [1, 2], [3, 3], [3, 5], [4, 0], [4, 6], [4, 8], [5, 0], [8, 2], [9, 2]]},
    {"noGoods": [[0, 0], [0, 4], [0, 5], [1, 4], [1, 9], [4, 5], [5, 3], [7, 1], [7, 2], [7, 8]]},
    {"noGoods": [[0, 2], [0, 8], [1, 6], [2, 0], [2, 7], [5, 0], [5, 1], [5, 4], [5, 9], [9, 0]]},
    {"noGoods": [[0, 5], [1, 1], [1, 8], [2, 4], [2, 8], [3, 5], [5, 3], [5, 6], [6, 3], [9, 9]]},
    {"noGoods": [[0, 7], [0, 8], [1, 4], [2, 0], [3, 0], [3, 2], [3, 6], [4, 1], [6, 1], [8, 8]]},
    {"noGoods": [[1, 8], [3, 1], [4, 8], [5, 5], [7, 8], [8, 2], [9, 2], [9, 3], [9, 7], [9, 9]]},
    {"noGoods": [[0, 1], [2, 1], [2, 2], [5, 8], [6, 9], [7, 4], [8, 4], [8, 7], [9, 0], [9, 8]]},
    {"noGoods": [[1, 4], [2, 4], [2, 7], [3, 3], [3, 9], [5, 0], [6, 8], [7, 0], [9, 0], [9, 6]]}
  ],
  "constraints": [
    {"id": 0, "vars": [17, 19]},
    {"id": 1, "vars": [57, 94]},
    {"id": 2, "vars": [10, 28]},
    {"id": 3, "vars": [1, 90]},
    {"id": 4, "vars": [55, 64]},
    {"id": 5, "vars": [9, 32]},
    {"id": 6, "vars": [3, 12]},
    {"id": 7, "vars": [52, 69]},
    {"id": 8, "vars": [11, 59]},
    {"id": 9, "vars": [14, 44]}
  ]
}
'''

def test_urbcsp(exe):
    r = run_cj_gen_urbcsp(exe, '100 10 10 10 100 99'.split(' '))
    assert r.returncode == 0
    assert r.stdout.decode('utf-8') == expected

def test_urbcsp_num_constraint_defs(exe):
    r = run_cj_gen_urbcsp(exe, '100 10 10 10 100 99 10'.split(' '))
    assert r.returncode == 0
    assert r.stdout.decode('utf-8') == expected
