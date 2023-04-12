#! /usr/bin/env python3

import subprocess
import os
import yaml
import shutil

TEST_PATH = os.path.dirname(os.path.abspath(__file__))
APROJ_EXE = TEST_PATH + "/../antler-proj"

def run_cmd(cmd):
    """ Execute `cmd`. """
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    return out.decode('utf-8'), err.decode('utf-8')

def antler_proj_cmd(args):
    """ Run antler-proj with `args`. """
    return run_cmd(APROJ_EXE + " " + args)

def antler_in_proj_cmd(path, args):
    """ Change directory to `path` and run antler-proj with `args`. """
    return run_cmd("cd " + path + "; " + APROJ_EXE + " " + args)

def load_project(path):
    with open("./{0}/project.yml".format(path), "r") as f:
        project = yaml.safe_load(f)
    return project

def expected_cmd(cmd, expected):
    out, err = run_cmd(cmd)
    if out != expected:
        print("ERROR: cmd: %s != expected: %s" % (cmd, expected))
        assert(False)

def expected_proj_cmd(cmd, expected):
    out, err = antler_proj_cmd(cmd)
    if out != expected:
        print("ERROR: cmd: %s != expected: %s" % (cmd, expected))
        assert(False)
