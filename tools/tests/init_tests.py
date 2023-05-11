#! /usr/bin/env python3

import sys
import shutil

from util import *

def init_project(project_name, version, path):
   out, err = antler_proj_cmd("init -p {0} -n {1} -v {2}".format(path, project_name, version))
   print(out)

def init_project_default_path(project_name, version):
   out, err = antler_proj_cmd("init -n {0} -v {1}".format(project_name, version))
   print(out)


def test_init_project():
   shutil.rmtree("test0", ignore_errors=True)
   init_project("test0", "v1.0.0", "./test0")
   assert(os.path.isdir("./test0"))
   assert(os.path.isdir("./test0/apps"))
   assert(os.path.isdir("./test0/include"))
   assert(os.path.isdir("./test0/ricardian"))
   assert(os.path.isdir("./test0/libs"))
   assert(os.path.isfile("./test0/project.yaml") or os.path.isfile("./test0/project.yml"))

   project = load_project("./test0")
   assert(project["project"] == "test0")
   assert(project["version"] == "1.0.0")

def test_init_project_2():
   shutil.rmtree("test1", ignore_errors=True)
   init_project("abc", "v1", "./test1")
   assert(os.path.isdir("./test1"))
   assert(os.path.isdir("./test1/apps"))
   assert(os.path.isdir("./test1/include"))
   assert(os.path.isdir("./test1/ricardian"))
   assert(os.path.isdir("./test1/libs"))
   assert(os.path.isfile("./test1/project.yaml") or os.path.isfile("./test1/project.yml"))

   project = load_project("./test1")
   assert(project["project"] == "abc")
   assert(project["version"] == "1.0.0")

def test_init_project_3():
   shutil.rmtree("test2", ignore_errors=True)
   init_project_default_path("abc", "v1")
   assert(os.path.isdir("./abc"))
   assert(os.path.isdir("./abc/apps"))
   assert(os.path.isdir("./abc/include"))
   assert(os.path.isdir("./abc/ricardian"))
   assert(os.path.isdir("./abc/libs"))
   assert(os.path.isfile("./abc/project.yaml") or os.path.isfile("./abc/project.yml"))

   project = load_project("./abc")
   assert(project["project"] == "abc")
   assert(project["version"] == "1.0.0")

if __name__ == "__main__":
   test_init_project()
   test_init_project_2()
   test_init_project_3()
