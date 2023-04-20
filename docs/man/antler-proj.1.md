---
title: antler-proj
section: 1
header: User Manual
footer: antler-proj 1.0.0
date: March 09, 2023
---
# NAME
antler-proj - Antler Project Management System

# SYNOPSIS

`antler-proj <command> [<args>]`

# DESCRIPTION

**Antler Project Management System** is a set of libraries and tools to help users successfully develop smart contracts.

# COMMANDS

This is a general help text which prints when antler-proj runs without a command or with key ---help.

**Example: `antler-proj --help`**

Each command has its own help which prints when the command runs with key ---help.

**Example: `antler-proj init --help`**

Below is described a full list of all possible commands and their possible arguments

**init** command creates a new project. Creating the directory tree, a `project.yaml` file and initializes it.

```
Command arguments:
    <path>                      Path to the root of the project. Here a `project.yaml` file will be created
    <project_name>              Name of the project
    [version]                   Version of the project. Default version is 0.0.1
    -h,--help                   Print this help message and exit
    -p <path>                   Path to the root of the project.
    -n <project_name>           The name of the project.
    -v <version>                The version to store in the project file.
```
**Example: `antler-proj init ./project hello_world 0.1`**


**add** command adds an app, dependency, library or test to your project.

```
Command arguments:
  -h,--help                   Print this help message and exit
  -p <path>                   This must be the path to the `project.yaml` or the path containing it.
  [path]                      Path to the root of the project.
  <app,lib,dep>               Type of an entity to work with
```
**Example: `antler-proj add ./project app`**


**update** command updates an app, dependency, library or test to your project.

```
Command arguments:
  -h,--help                   Print this help message and exit
  -p <path>                   This must be the path to the `project.yaml` or the path containing it.
  [path]                      Path to the root of the project.
  <app,lib,dep>               Type of an entity to work with
```
**Example: `antler-proj update ./project app`**


**remove** command removes an app, dependency, library or test from your project.

```
Command arguments:
  -h,--help                   Print this help message and exit
  -p <path>                   This must be the path to the `project.yaml` or the path containing it.
  [path]                      Path to the root of the project.
  <app,lib,dep>               Type of an entity to work with
```
**Example: `antler-proj remove ./project app`**


**populate** command loads existing information about the project starting from the root project.yaml file as described in the chapter `"Loading of a project"` in the User's guide.

    Without additional keys it passes through a subtree of each application and checks all non-local dependencies for updates.
    Downloads all the updates in a way depending on where it is located.

```
Command arguments:
    -h,--help                   Print this help message and exit
    -p <path>                   This must be the path to the `project.yml` or the path containing it.
    [path]                      Path to the root of the project.
```
**Example: `antler-proj populate ./project`**

**validate** command performs a check of the project to ensure that all data that describes the project is correct.

    Check all project.yaml files described in the project for updates.
    Download the updated project.yaml files.
    Perform all checks of the project as described in the user's guide.
    Check if it is possible to access all dependencies. I.e. if all described local directories of all local dependencies
    exist and if it is possible to download all remote dependencies.

```
Command arguments:
    -h,--help                   Print this help message and exit
    -p <path>                   Path to the root of the project.
    -V,--verbose                Verbose output.
```
**Example: `antler-proj validate ./project`**

**build** command builds all applications described in the project.yaml, their dependencies and tests.

```
Command arguments:
    -h,--help                   Print this help message and exit
    -p <path>                   This must be the path to the `project.yml` or the path containing it.
    [path]                      Path to the root of the project.
```
**Example: `antler-proj build ./project`**


**`--version`**                     Returns the version of antler-proj application
**Example: `antler-proj --version`**

# EXAMPLES

```
  antler-proj add app MyApp C++ \\-O2
  antler-proj add app -n MyApp -l C++ "\-O2 -WError" \\-s
  antler-proj add lib MyLib C++ \\-O2 "\-s"
  antler-proj add dep MyApp MyDep
  antler-proj build -j3
  antler-proj init MyProjectName 1.0.0
  antler-proj populate ./path-to-project
  antler-proj remove app MyApp
  antler-proj remove lib MyLib
  antler-proj remove dep MyDep MyApp
  antler-proj update app MyApp C++ \\-O2
  antler-proj update lib MyLib C++ \\-O2 "\-s"
  antler-proj update dep MyDep -l AntelopeIO/my_dep
  antler-proj validate
```

# AUTHORS

# BUGS
Please submit bug reports online at: <https://github.com/AntelopeIO/antler-proj/issues>

# SEE ALSO
Full documentation and sources at: <https://github.com/AntelopeIO/antler-proj>
