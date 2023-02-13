# User's guide

##### For antler-proj Project Management System (AKA aproj) CLI

In this guide we will learn how to use aproj to simplify and speedup of development of the contract projects. 
If you want to learn abilities of aporj, it's commands, how it works and how it designed please read this document https://github.com/AntelopeIO/antler-proj/blob/main/docs/usecases.md

In order to show why it make sense to use aproj we take examples from EOS tutorials and show how to create and manage these projects with aproj.

The general way of using of aptoj looks like this:

- ##### We ask aproj to create a new project for us.

Project is a complex of applications which implements the smart contracts and other kinds of programs, programming libraries and other dependencies which needed for building of the applications, and tests needed for testing of the applications and the libraries.

- ##### We ask aproj to add a new application, library or test to the project.

By default aproj creates a set of empty directories, project files and building files enough for start of the building process. 
But we may ask aproj add a new application, library or test from templates of the template library of aproj.
In this case it adds files where implemented frequently used project structures and functions. 
Or even you can create your own template which implements all needed features which you often use.

- ##### We add to the project descriptions of all dependencies needed building for our applications, libraries or tests

Like external libraries or/and data files.

- ##### We ask aproj to download all external dependencies and build all part of the project.

And here we have binaries of the smart contracts ready for deploying into the block chain and work



## Prerequisites

Lets say we have successfully installed EOS tools and the test network is already bootstrapped and ready for deploying of the user's contacts.

## Understanding of the templates

Aproj has a library of templates. They allows to make a stub of any kind of application and use it as a good base for a new project where the frequently used code is already implemented. You won't need to copy it from one project to another and manually edit names of files, classes and variables which are different in different projects.

The library of templates is organized very simple. It is a set of directories. Each directory contains directory structure of a project with all needed files for this project. All the texts inside of the files which may be changed are described as macros. Creation of a new project from a template have following algorithm:
 
0. User runs a command: aproj ./project_path add --app --template template_name
 
1. Copy of a directories and files from the chosen template into the project directory; 
2. Change names of files and directories corresponding to name of the project 
3. Process each file with macro processor to substitute macros to real data in the files. Any user may  create his/her own templates by copying and editing of existing templates and adding of a new directory with the new template to the template storage.

Later in this guide we will learn this processes more details and create a new template from an existing template.

## Hello World!

Let's see how to create and manage a project in aproj on the example of smart contract Hello World, described in EOS Tutorial here https://docs.eosnetwork.com/docs/latest/getting-started/smart-contract-development/hello-world/

The tutorial has a detailed description of how to write, build and deploy the contract from scratch. Aproj is a more hight level tool designed to avoid manual work as much as possible. So here we will not create this project from scratch but create if from an existing template. This template calls "basic" and contains an implementation of a contract with one action. So after creation of the project from this template the user need only feel the method of the action by his own logic. We will write there the code which will prints "Hello <user>" as in the tutorial.

## The Creation

> _In the beginning was the Word, and the Word was with two bytes._

We need to create a directory with an empty project structure into this directory. So let's call

`aproj init --name project`

This call creates a new directory "project" in the current directory with an empty project structure.
Optional key `--ver` may be used to define an initial version of the project. 

For example: `aproj init --name project --ver 1.0`

If the initial version of the project is not explicitly defined it is set to 0.0. 

```
──project
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
    ├───tests
    └───project.yaml
```

This is the structure of an empty project. It contains a `project.yaml` file with description of the project and 
directories where will be created applications, programming libraries and test for these applications and
libraries.


## Adding of a new Application

In order to add a new application we should call the following command:

`aproj add ./project --app --name hello --template basic`

This call creates an application's directory `hello` in the project's directory `apps`
 
```
apps/
└── hello
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── hello.hpp
    ├── README.txt
    ├── ricardian
    │   └── hello.contracts.md
    └── src
        ├── CMakeLists.txt
        └── hello.cpp
```

with all needed files for building of the project. FIle `hello.cpp` has implementation of a contract with
and empty action:

```c++
#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] hello : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void my_action() {
        // Write implementation of your action here 
      }
};
```

We may rename the action and add the printing of a message like in the example from EOS tutorials https://docs.eosnetwork.com/docs/latest/getting-started/smart-contract-development/hello-world/
                                                                                                  
```c++
#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] hello : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void hi( name user ) {
         print( "Hello, ", user);
      }
};
```

## Building of a new Application

It time to build our app.

Call `aproj build ./project`

builds all libraries applications and tests described in the project.

Call `aproj build ./project --app --name hello` 
builds only an application with name hello.

If you need to rebuild the app or whole project from scratch, call:

`aproj clean ./project && aproj build ./project` 

or 

`aproj clean ./project --app --name hello && aproj build ./project --app --name hello`

for the application `hello` only


## Deploying of a contract

When our contract is built we should deploy it to out block chain.
It is easy to do with `DUNE`

Call `dune --deploy ./projects/apps/hello/build/hello/ eosio`

Then let's call the action from the deployed contract.

Call 'dune --send-action eosio hello hi '["bob"] bob@active'

