#! /usr/bin/env python3

""" Test `antler-proj add` and `antler proj update` commands. """

import os
import shutil

from util import antler_in_proj_cmd, load_project
from init_tests import init_project

PROJECT_NAME="aou_project"
PROJECT_PATH=os.path.join("./",PROJECT_NAME)

def expect_subvalue(array_of_lists, name, tag, value):
    """ Inside array_of_lists, find the list that contains `name` and test the value
        stored in that list for the given tag against value.


    :param array_of_lists  An array of lists.
    :param name  The name to search for.
    :tag  The tag that contains the value we to test.
    :value  The expected value.

    """

    for i in array_of_lists:
        if i["name"] == name:
            return i[tag] == value
    return False

def find_app(yaml, app_name):
    apps = yaml["apps"]
    for app in apps:
        if app["name"] == app_name:
            return app

def setup_project():
    """Setup the project ensuring there is a fresh project file.
    """

    shutil.rmtree(PROJECT_PATH, ignore_errors=True)
    init_project(PROJECT_NAME, "v1.0.0", PROJECT_PATH)
    assert os.path.isdir(PROJECT_PATH)


def test_app_compile_options():
    """Add an application to the project and test its compile options can be updated.
    """

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add app -n App1 -l C++ --comp \\\\-O1")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["apps"], "App1", "compile_options", "-O1")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App1 -l C++ --comp \\\\-O2")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["apps"], "App1", "compile_options", "-O2")


def test_app_link_options():
    """Add an application to the project and test its compile and link options can be updated.
    """

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add app -n App2 -l C++ --comp \"\\-O1\" --link \\\\-t")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["apps"], "App2", "compile_options", "-O1")
    assert expect_subvalue(project["apps"], "App2", "link_options", "-t")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App2 -l C++ --comp \"\\-O2\" --link \\\\-s")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["apps"], "App2", "compile_options", "-O2")
    assert expect_subvalue(project["apps"], "App2", "link_options", "-s")


def test_app_update_options():
    """Add an application to the project and test its compile and link options
       can be updated with flags.
    """

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add app -n App3 -l C++")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["apps"], "App3", "compile_options", "")
    assert expect_subvalue(project["apps"], "App3", "link_options", "")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App3  --comp \\\\-O1")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App3", "compile_options", "-O1")
    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App3  --comp \"\\-O2\"")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App3", "compile_options", "-O2")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App3  --link \\\\-s")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App3", "link_options", "-s")
    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update app -n App3  --link \"\\-t\"")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App3", "link_options", "-t")

    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App3", "compile_options", "-O2")

    # Test for a missing application
    out, err = antler_in_proj_cmd(PROJECT_PATH, "update app -n NonexistingApp  --comp \\\\-O1")
    print(out)
    print(err)
    assert "Object NonexistingApp does not exist" in err


def test_lib_options():
    """Add a library to the project and test its compile and link options can be updated with flags.
    """

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add lib -n Lib1 -l C++")
    print(out)
    project = load_project(PROJECT_PATH)
    assert expect_subvalue(project["libs"], "Lib1", "compile_options", "")
    assert expect_subvalue(project["libs"], "Lib1", "link_options", "")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update lib -n Lib1  --comp -O1")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["libs"], "Lib1", "compile_options", "-O1")
    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update lib -n Lib1  --comp \"-O2\"")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["libs"], "Lib1", "compile_options", "-O2")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update lib -n Lib1  --link -s")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["libs"], "Lib1", "link_options", "-s")
    out, _ = antler_in_proj_cmd(PROJECT_PATH, "update lib -n Lib1  --link \"-t\"")
    print(out)
    assert expect_subvalue(load_project(PROJECT_PATH)["libs"], "Lib1", "link_options", "-t")

    assert expect_subvalue(load_project(PROJECT_PATH)["libs"], "Lib1", "compile_options", "-O2")

def test_add_dep_options():
    """Adds deepndency to projects application and test if it appears in yaml file
    """
    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add app -n App4 -l C++ --comp \\\\-O3")
    print(out)
    project = load_project(PROJECT_PATH)
    
    assert expect_subvalue(load_project(PROJECT_PATH)["apps"], "App4", "compile_options", "-O3")

    out, _ = antler_in_proj_cmd(os.path.join(PROJECT_PATH, "apps/App4"), "add dep -u https://github.com/larryk85/cturtle")
    print(out)

    app4 = find_app(load_project(PROJECT_PATH), "App4")
    assert app4 is not None

    assert expect_subvalue(app4["depends"], "cturtle", "location", "larryk85/cturtle")

    out, _ = antler_in_proj_cmd(os.path.join(PROJECT_PATH, "apps/App4"), "add dep -n Dep4 -u https://github.com/larryk85/test_repo")
    print(out)

    app4 = find_app(load_project(PROJECT_PATH), "App4")
    assert app4 is not None

    assert expect_subvalue(app4["depends"], "Dep4", "location", "larryk85/test_repo")

    out, _ = antler_in_proj_cmd(PROJECT_PATH, "add dep --to App4 -u https://github.com/larryk85/sample_contract")
    print(out)

    app4 = find_app(load_project(PROJECT_PATH), "App4")
    assert app4 is not None
    
    assert expect_subvalue(app4["depends"], "sample_contract", "location", "larryk85/sample_contract")

if __name__ == "__main__":
    setup_project()
    test_app_compile_options()
    test_app_link_options()
    test_app_update_options()
    test_lib_options()
    test_add_dep_options()
