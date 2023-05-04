# User's guide for antler-proj Project Management System CLI

## Project Creation

We need to create a directory with an empty project structure. So let's call

`antler-proj init <directory for project> <name of project> <version>`

This call creates a new directory "project" in the directory specified.

If the initial version of the project is not explicitly defined, it is set to 0.0.0 

```
──project
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
    └───project.yaml
```

This is the structure of an empty project. It contains a `project.yaml` file with a description of the project and 
directories where it will create applications, programming libraries and tests for them.


## Adding of a new application

Run the command `antler-proj add app <application name> <language (C++ is currently only supported)> <optional compile options> <optional link options>`

This will add a new directory under the apps and include directories with the name of that app, along with a new header file and source file ready to go for development.

Example usage:

`antler-proj add app foo C++`

The directory tree will look like this:
```
──project
    ├───apps
        ├───foo
            ├───foo.cpp
    ├───include  
        ├───foo
          ├───foo.hpp
    ├───ricardian
    ├───libs
    └───project.yaml
```

And the `project.yaml` will look something like this:
```yaml
project: project
version: 1.0.0
apps:
   - name: foo
     lang: CXX
     compile_options: ""
     link_options: ""
```

You can have many smart contract applications within one project.

## Adding of a new library

Run the command `antler-proj add lib <library name> <language (C and C++ currently supported)> <optional compile options> <optional link options>`

This will add a new directory under the libs directory with the name of that lib and a source file ready to go for development.

Example usage:

`antler-proj add lib foolib C`

The directory tree will look like this:
```
──project
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
        ├───foolib
          ├───foolib.c
    └───project.yaml
```
And the `project.yaml` will look something like this:
```yaml
project: project
version: 1.0.0
libs:
   - name: foolib
     lang: C
     compile_options: ""
     link_options: ""
```

You can have any number of libraries defined and created within the project.

## Using libraries or smart contracts as dependencies.

In your project you can declare any library or app as a dependency of any other library or app that you are creating.

This is done by using the command `antler-proj app dep <app/lib name to attach dependency to> <dependency name>`.

In this usage `<dependency name>` should be the same name of the app or lib that you are wanting to depend upon.

Example usage:
`antler-proj add dep foo foolib`

This doesn't add any new directories or files, but updates the `project.yaml`.

And the `project.yaml` will look something like this:
```yaml
project: project
version: 1.0.0
apps:
   - name: foo
     lang: CXX
     compile_options: ""
     link_options: ""
     depends:
        - name: foolib
          location: ""
          tag: ""
          release: ""
          hash: ""
libs:
   - name: foolib
     lang: C
     compile_options: ""
     link_options: ""
```

## Using external libraries or smart contracts as dependencies.

In your project you can declare any external library or app as a dependency of any other library or app that you are creating.

The external dependencies should be of the form of an `antler-proj` project.  They must also be on Github for the first version of this software, those restrictions will be removed in the next release.

This is done by using the command `antler-proj app dep <app/lib name to attach dependency to> <dependency name> <location> <optional tag> <optional release value>`.

In this usage `<dependency name>` should be the same name of the app or lib that you are wanting to depend upon. `<location>` should be of the form `org/repo` (i.e. antelopeio/antler-proj), the `optional tag` is used to pull a specific version tag or commit hash to depend on.  The `optional release value` is used to define if you want a specific release that was 'released' on Github.

This will retreive the dependencies required to satisfy the requirements.

Example usage:
`antler-proj add dep foo token antelopeio/antler-proj-token main`

This doesn't add any new directories or files, but updates the `project.yaml`.

And the `project.yaml` will look something like this:
```yaml
project: project
version: 1.0.0
apps:
   - name: foo
     lang: CXX
     compile_options: ""
     link_options: ""
     depends:
        - name: token
          location: "antelopeio/antler-proj-token"
          tag: "main"
          release: ""
          hash: ""
```

## Building the project

It is time to build our app.

Call `antler-proj build` is all that is needed if you are in the project, if not specify the project directory with `-p <path>`.
If you need to rebuild the app or whole project from scratch, call:

`antler-proj build --clean` will rebuild the project and do a full clean of the project before hand.  As with above use the `-p` flag to specify the path to the root of the project.