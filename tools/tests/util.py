#! /usr/bin/env python3

"""Utility functions for antler-proj testing."""

import subprocess
import os
import yaml

TEST_PATH = os.path.dirname(os.path.abspath(__file__))
APROJ_EXE = TEST_PATH + "/../antler-proj"

def run_cmd(cmd):
    """ Execute `cmd`. """
    result = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = result.communicate()
    return out.decode('utf-8'), err.decode('utf-8')

def antler_proj_cmd(args):
    """ Run antler-proj with `args`. """
    return run_cmd(APROJ_EXE + " " + args)

def antler_in_proj_cmd(path, args):
    """ Change directory to `path` and run antler-proj with `args`. """
    return run_cmd("cd " + path + "; " + APROJ_EXE + " " + args)

def load_project(path):
    """ Load a project.yml file """
    with open("./{0}/project.yml".format(path), "r") as file_handle:
        project = yaml.safe_load(file_handle)
    return project

def expected_cmd(cmd, expected):
    """ Test that cmd has an expected result. """
    out, _ = run_cmd(cmd)
    if out != expected:
        print("ERROR: cmd: %s != expected: %s" % (cmd, expected))
        assert False

def expected_proj_cmd(cmd, expected):
    """ Test that `antler-proj cmd` has an expected result. """
    out, _ = antler_proj_cmd(cmd)
    if out != expected:
        print("ERROR: cmd: %s != expected: %s" % (cmd, expected))
        assert False
