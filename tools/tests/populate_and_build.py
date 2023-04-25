#! /usr/bin/env python3

""" Test `antler-proj add` and `antler proj update` commands. """

import os
import shutil
import subprocess

from util import APROJ_EXE

PROJECT_NAME="pandb_contract"
PROJECT_PATH=os.path.join("./",PROJECT_NAME)
SOURCE_PATH=os.path.join("@CMAKE_CURRENT_SOURCE_DIR@/tests",PROJECT_NAME)


def test_populate_and_build():
    """ Setup a clean project, then populate and build it.
    """
    # Remove any existing test artifact, then copy the test project over.
    shutil.rmtree(PROJECT_PATH, ignore_errors=True)
    assert shutil.copytree(SOURCE_PATH, PROJECT_PATH)

    # Populate the project.
    result = subprocess.run([APROJ_EXE,"populate", PROJECT_PATH], capture_output=True, text=True, check=False)
    print(result.stdout)
    print(result.stderr)
    assert result.returncode == 0

    # Build the project.
    result = subprocess.run([APROJ_EXE,"build", PROJECT_PATH], capture_output=True, text=True, check=False)
    print(result.stdout)
    print(result.stderr)
    assert result.returncode == 0


if __name__ == "__main__":
    test_populate_and_build()
