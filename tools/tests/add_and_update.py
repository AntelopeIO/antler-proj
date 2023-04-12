#! /usr/bin/env python3

import sys

from util import *
from init_tests import init_project

project_name="aou_project"
project_path=os.path.join("./",project_name)

def expect_subvalue(ll, name, tag, value):
   """Inside ll, find the list that contains `name` and test the value stored in that list for the given tag against value.

   :param ll  An array of lists.
   :param name  The name to search for.
   :tag  The tag that contains the value we to test.
   :value  The expected value.
   """

   for i in ll:
      if i["name"] == name:
         return i[tag] == value
   return False


def setup_project():
   """Setup the project ensuring there is a fresh project file.
   """

   shutil.rmtree(project_path, ignore_errors=True)
   init_project(project_name, "v1.0.0", project_path)
   assert(os.path.isdir(project_path))


def test_app_compile_options():
   """Add an application to the project and test its compile options can be updated.
   """

   out, err = antler_in_proj_cmd(project_path, "add app App1 C++ \\\\-O1")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["apps"], "App1", "compile_options", "-O1") )

   out, err = antler_in_proj_cmd(project_path, "update app App1 C++ \\\\-O2")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["apps"], "App1", "compile_options", "-O2") )


def test_app_link_options():
   """Add an application to the project and test its compile and link options can be updated.
   """

   out, err = antler_in_proj_cmd(project_path, "add app App2 C++ \"\\-O1\" \\\\-t")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["apps"], "App2", "compile_options", "-O1") )
   assert( expect_subvalue(project["apps"], "App2", "link_options", "-t") )

   out, err = antler_in_proj_cmd(project_path, "update app App2 C++ \"\\-O2\" \\\\-s")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["apps"], "App2", "compile_options", "-O2") )
   assert( expect_subvalue(project["apps"], "App2", "link_options", "-s") )


def test_app_update_options():
   """Add an application to the project and test its compile and link options can be updated with flags.
   """

   out, err = antler_in_proj_cmd(project_path, "add app App3 C++")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["apps"], "App3", "compile_options", "") )
   assert( expect_subvalue(project["apps"], "App3", "link_options", "") )

   out, err = antler_in_proj_cmd(project_path, "update app App3  --comp \\\\-O1")
   print(out)
   assert( expect_subvalue(load_project(project_path)["apps"], "App3", "compile_options", "-O1") )
   out, err = antler_in_proj_cmd(project_path, "update app App3  --comp \"\\-O2\"")
   print(out)
   assert( expect_subvalue(load_project(project_path)["apps"], "App3", "compile_options", "-O2") )

   out, err = antler_in_proj_cmd(project_path, "update app App3  --link \\\\-s")
   print(out)
   assert( expect_subvalue(load_project(project_path)["apps"], "App3", "link_options", "-s") )
   out, err = antler_in_proj_cmd(project_path, "update app App3  --link \"\\-t\"")
   print(out)
   assert( expect_subvalue(load_project(project_path)["apps"], "App3", "link_options", "-t") )

   assert( expect_subvalue(load_project(project_path)["apps"], "App3", "compile_options", "-O2") )


def test_lib_options():
   """Add a library to the project and test its compile and link options can be updated with flags.
   """

   out, err = antler_in_proj_cmd(project_path, "add lib Lib1 C++")
   print(out)
   project = load_project(project_path)
   assert( expect_subvalue(project["libs"], "Lib1", "compile_options", "") )
   assert( expect_subvalue(project["libs"], "Lib1", "link_options", "") )

   out, err = antler_in_proj_cmd(project_path, "update lib Lib1  --comp -O1")
   print(out)
   assert( expect_subvalue(load_project(project_path)["libs"], "Lib1", "compile_options", "-O1") )
   out, err = antler_in_proj_cmd(project_path, "update lib Lib1  --comp \"-O2\"")
   print(out)
   assert( expect_subvalue(load_project(project_path)["libs"], "Lib1", "compile_options", "-O2") )

   out, err = antler_in_proj_cmd(project_path, "update lib Lib1  --link -s")
   print(out)
   assert( expect_subvalue(load_project(project_path)["libs"], "Lib1", "link_options", "-s") )
   out, err = antler_in_proj_cmd(project_path, "update lib Lib1  --link \"-t\"")
   print(out)
   assert( expect_subvalue(load_project(project_path)["libs"], "Lib1", "link_options", "-t") )

   assert( expect_subvalue(load_project(project_path)["libs"], "Lib1", "compile_options", "-O2") )





if __name__ == "__main__":
   setup_project()
   test_app_compile_options()
   test_app_link_options()
   test_app_update_options()
   test_lib_options()
