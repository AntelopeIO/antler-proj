#! /usr/bin/env python3

""" Test `antler-proj add` and `antler proj update` commands. """

import os
import shutil
import subprocess

from util import APROJ_EXE

PROJECT_NAME="pandb_contract"
PROJECT_PATH=os.path.join("./",PROJECT_NAME)
SOURCE_PATH=os.path.join("@CMAKE_CURRENT_SOURCE_DIR@/tests",PROJECT_NAME)

def warn_cdt_not_installed():
    txt_warning = 'Warning: Skip the test on the antler-proj build command because CDT is not installed properly in your system!'
    # pylint: disable=consider-using-f-string
    print("\033[93m {}\033[00m".format(txt_warning))  # print txt_warning in yellow

def test_populate_and_build():
    """ Setup a clean project, then populate and build it.
    """
    # Remove any existing test artifact, then copy the test project over.
    shutil.rmtree(PROJECT_PATH, ignore_errors=True)
    assert shutil.copytree(SOURCE_PATH, PROJECT_PATH)

    # Populate the project.
    result = subprocess.run([APROJ_EXE,"populate", "-p", PROJECT_PATH], stdout=subprocess.PIPE,stderr=subprocess.PIPE,  check=False)
    print(result.stdout)
    print(result.stderr)
    assert result.returncode == 0

    # Build the project.
    result = subprocess.run([APROJ_EXE,"build", "-p", PROJECT_PATH], stdout=subprocess.PIPE,stderr=subprocess.PIPE,  check=False)
    print(result.stdout)
    print(result.stderr)
    if b'CDT is not installed' in result.stderr:
        warn_cdt_not_installed()
    else:
        assert result.returncode == 0

    result = subprocess.run([APROJ_EXE,"build", "-p", PROJECT_PATH, "-j", "1"], stdout=subprocess.PIPE,stderr=subprocess.PIPE,  check=False)
    print(result.stdout)
    print(result.stderr)
    if b'CDT is not installed' in result.stderr:
        warn_cdt_not_installed()
    else:
        assert result.returncode == 0


if __name__ == "__main__":
    test_populate_and_build()
