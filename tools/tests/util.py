#! /usr/bin/env python3

import subprocess

def run_cmd(cmd):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    return out.decode('utf-8'), err.decode('utf-8')
   
def expected(cmd, expected):
    out, err = run_cmd(cmd)
    if out != expected:
        print("ERROR: cmd: %s != expected: %s" % (cmd, expected))
        assert(False)