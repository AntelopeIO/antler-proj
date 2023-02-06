## Project Management System “antler-proj” ( AKA aproj, the system) 
### Use cases for CLI

##### General provisions: 

Format of the command line: `aproj <command> [path to the project file] [--key [param]  [--key [param] ] ]` 

There is a general help text which prints when `aproj` runs without a command or with key `--help`. 

Example: `aproj --help`

Each command has it's own help which prints when the command runs with key `--help`. 

Example: `aprog init --help`

`<command>` is a string without spaces. Key is a string started from double dash. Parameters of a key following the key. 

Example: `aproj init --name project_name`

`[path to the project file]` (string) Path in the local file system where the project files and directories are placed or will be created depending of a command.

Error handling strategy: logging of the process of work, exit when meet an error. 

Localization: Must support local symbols in names of apps, libs, tests, directories and files which the system manipulate with.
All the names are UTF8 strings which may contain multibyte symbols. Max length of each name is 256 symbols.

### Glossary:

`Project` is a directory structure which contains description of the project project.yml file in YAML (https://yaml.org/)  format, Description of applications and needed for their building libraries and other dependencies.

`Application` (App) is a binary (usually a smart contract) which built by description from project.yml, libraries and other dependencies.  

`Test` is a binary or script for testing of an application linked with this test

`Library` (Lib) is a source code or binary files of a programming language library needed for building of an Application. Libraries may be two types: 1. GitHub repository, 2, Local directory contains. All of the types must have a project.yaml file with description of the library and set of files needed for building.

`Version`: Each entity described in the project file must have its version. Version is a string of numbers separated with dots. In following format: <number>[.number[.number] ] Examples: "1.0.3", "34", "0.1". "000.0.1". 
Comparison of two versions is carried out by consecutive comparison of numbers from right to left.  

## Creation of a new project

Command `"init"` creates a new empty project.

User run in a command line `"aproj"` with command `"init"` and the following parameters:

`--name <Project name>` (string)

`--ver <Project version>` (string)

As result of work an example of the command line: 

`aproj  init ./projects --name example --ver 1.0`

The system generates a YAML file `"project.yaml"` in format:
```
   project: "example"
   version: "1.0"
   libraries:
   apps:
```
Creates a directory structure:

```
──project_root (./projects/example)
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
    ├───tests
    └───project.yaml
```

## Adding of a new lib to the project

Command `"add"` with key `--lib` adds a description of a new library to a project.

User run in a command line `aproj` with command `"add"` and following parameters:

`--name <name of the lib>` (string)

`--ver <version of the lib>` (string)

`--lang <programming language of the lib>` (string)

`--build <command>` (string) command which must be run to build the library
 
`--from <URL>` path to the library files or a Git repository
 
`--dep <dep name [dep name]>` dependencies of the library (list of strings with names of dependencies which must be already described in the project file)

As result of work an example of the command line: 

```
aproj add ./projects/example --lib --name lib2 --ver "3.0.1" --lang "C" --build "CMake ." --from "./projects/example/libs/lib2" &&
aproj add ./projects/example --lib --name lib1 --ver "0.2.1" --dep lib2 --lang "C++" --build "make -j 2" --from "https://github.com/larryk85/fast_math"
```

The system generates a YAML file "project.yaml" in format:

```
   project: "example"
   version: "1.0"
   libraries:
     - name: "lib2"
       lang: "C"
       build: "CMake ."
       from: "file://home/me/projects/example/libs/lib2"
       version: 3.0.1
     - name: "lib1"
       lang: "C++"
       build: "make -j 2"
       from: "https://github.com/larryk85/fast_math"
       version: 0.2.1
       depends: 
         - name: lib2
   apps:
```

Creates a directory structure:

```
──project_root (./projects/example)
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
    ├───libs
          └──lib1/
		└── project.yaml
          └──lib2/
		└── project.yaml
    ├───tests
    └───project.yaml
```



## Adding of a new app to the project

Command `"add"` with key `--app` adds a description of a new application to a project.

User run in a command line `aproj` with command `"add"` and following parameters:
      
`--name <name of the app>` (string)

`--ver <version of the lib>` (string)

`--template [template name]` (string) Create an fully functional application from a template. Without a template name creates a a generic implementation of a smart contract with one implemented action.

Abilities of this key may be extended to support a library of templates as described here: https://github.com/AntelopeIO/antler-proj/issues/2
  
`--lang <programming language of the app>` (string)
 
`--build <command>` (string) command which must be run to build the application
  
`--dep <dep name [dep name]>` dependencies of the app (list of strings with names of dependencies which will be created later)




As result of work an example of the command line :

``` 
aproj add ./projects/example --app --name app1 --ver "1" --lang "C++" --build "CMake ." --from "./projects/example/apps/app1" 
aproj add ./projects/example --app --name generic --template
```
 
The system generates a YAML file "project.yml" in format:

```yaml
   project: "example"
   version: "1.0"
   libraries:
     - name: "lib2"
       lang: "C"
       build: "cmake . && make"
       from: "file://home/me/projects/example/libs/lib2"
       version: 3.0.1
     - name: "lib1"
       lang: "C++"
       build: "make -j 2"
       from: "https://github.com/larryk85/fast_math"
       version: 0.2.1
       depends: 
     - name: lib2
    apps:
      - name: "app1"
        lang: "C++"
        version: 1
        build: "cmake . && make"
        depends:
      - name: "lib1"
      - name: "generic"
        lang: "C++"
        version: 1
        build: "cmake . && make"
```

Creates a subtree for a project in ./projects/example/apps
```
──project_root (./projects/example)
    ├───apps
          └──app1/
		└── project.yaml
	└──generic/
		├── build
		├── CMakeLists.txt
		├── include
		│   └── generic.hpp
		├── README.txt
		├── ricardian
		│	└── generic.contracts.md
		└── src
		    ├── CMakeLists.txt
		    ├── project.yaml
		    └── generic.cpp
    ├───include  
    ├───ricardian
    ├───libs
          └──lib1/
		└── project.yaml
          └──lib2/
		└── project.yaml
    ├───tests
    └───project.yml
```



## Loading of a project  

description of the internal process of loading information about the project into a project tree for following processing in other algorithms.

- Before start to execute most of the commands full information about the project from all project.yaml files must be loaded to a tree of dependencies corresponding to their relations.
 
- The tree of the project should be built in memory from the local project files placed in the project directory tree.

- Project files may be updated before the loading by command "populate" which downloads all new or changed files of all dependencies of the project

- When the tree is built it must pass following checks:
  - Connectivity check
  - Check that a graph of dependencies of each application is a tree
  - Check for cycling dependencies
  - Check correctness of all data in descriptions of entities.
  - Check if required versions of the dependencies fit to the available versions. 
- Trees of dependencies of each application should be topologically sorted to get the right order of building.
- The project is loaded and ready for processing.

In case of any error in the checks print detailed information about the problem to the log and to the console which should be enough to correct the error by the user.



## Populating of a project 

Command `"populate"` do downloading of all dependencies and preparing the project for building.

User run in a command line `aproj` with command `"populate"` and following parameters:

`--name <app name>` if this key is set do downloading only for this project

`--check` (optional) Do all checks as described in [Loading of a project](#Loading-of-a-project) but instead of downloading of the found updates just prints their list to the console.

`--reload` (optional) Remove all downloaded dependencies and download everything again

`--check` and `--reload` can't be used together, the attempt should generates an error.

Load existing information about the project starting from the root `project.yaml` file as it described in chapter [Loading of a project](#Loading-of-a-project). 

Without additional keys pass through a subtree of each application and check all non local dependencies for updates. Download all the updates in a way depending on where it is located.

Example: `aproj populate ./projects/example`



## Cleaning of a project

Command `"clean"` delete all files created during a building process.

User run in a command line `aproj` with command `"clean"` and following parameters:

`--name <name of app/lib/test>` Delete all files created during a building process for the only given project.

Without additional keys delete all files created during a building process for all projects. 

Example: `aproj clean ./projects/example`


## Validation of a project

Command `"validate"` do check of the project to make sure that all data which describes of the project is correct.

User run in a command line `aproj` with command `"validate"`


Check all project.yaml files described in the project for existing of updates.
Download the updated project.yaml files. 
Do all checks as described in [Loading of a project](#Loading-of-a-project)
Checks if possible to access to all dependencies. I.e. if all described local directories of all local dependencies 
are exist and if possible to download all remote dependencies.  


## Building of a new project

Command `"build"` do building of a given project and all its dependencies.

User run in a command line `aproj` with command `"build"` and following parameters:

`--name <name of app>` if this key is set do building only for this project and its dependencies.
`--ignore_errors` if this key is set don't stop building in case of non successful building of any subproject

Without additional keys do building of all applications described in the project.yaml, their dependencies and tests. 

For start of the building run a command described in field `"build"` of given project in a root directory of this project.

Check the return code of the building process. In case of non zero code generate an error and stop if `--ignore_errors` key is not set.

Before start of the building command intercept stdio and stderr of the building process and save them in a log file `"project_name.log"`
  

Example: `aproj build ./projects/example`


