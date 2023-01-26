## Project Manager
The project manager system will define a convention and specification for Antelope Smart Contract Packages.

### New tool antler-pack / cdt-pack
This tool will have functionality to create a new project and generate the correct directory tree and stub files.

This tool will be used to pull other packages dependencies listed in the manifest.

This tool will allow the contract writer to add new dependencies and sub sections of their app.

Github will act as the primary 'repo' for packages to exist.  When listing dependencies a few options are available:
1) Full URL of git project
2) Shorthand i.e. Org/Repo (example: antelopeio/safe_math)
3) An optional version can be added with the additional qualifier of greater than '>'

#### Manifest
Example project.yml:
```yaml
---
   project: "antelope_system_contract"
   version: "1.0.0"
   libraries:
     - name: "core"
       lang: "C++"
       options: "-O0"
     - name: "something"
       lang: "C"
       options: "-fno-something"
   apps:
      - name: "sys_contract"
        lang: "C++"
        depends:
           - name: "core"
           - name: "something"
           - name: "fast_math/math"
             from: "https://github.com/larryk85/fast_math"
             version: 3.0.1
           - name: "SomeLibrary/lib"
             from: "AntelopeIO/SomeLibrary"
             version: 1.0.2
      - name: "other"
        lang: "https://github.com/JavaJava/Java"
        depends:
           - name: "JavaLib/JavaLib"
             from: "https://github.com/JavaJava/JavaLib"
             version: 10.3.4
   tests:
      - name: "main_tests"
        depends:
           - name: "core"
           - name: "something"
        command: "./main_tests"
      - name: "functional_tests"
        depends:
           - name: "sys_contract"
           - name: "Catch2/Catch2"
             from: "catchorg/Catch2"
        command: "./functional_tests -verbose=0"
```

The directory structure for the project manager system will be as such:
```
──project_root
    ├───apps
    ├───include
    ├───ricardian
    ├───libs
    ├───tests
    └───project.yml
```
### Convention and Specification
A strict convention will be used to define packages/projects in terms of directory layout and manifest files.

### antler-pack --init
`antler-pack --init` should produce an initial project for the developer with the above given directory structure and project.yml.

Prompts should be given for:
- Project name
- Project version
- App/s this project will create
   - name of the app
   - programming language of the app
   - dependencies of the app
- Lib/s this project will create
   - name of the library
   - programming language of the library
   - dependencies of the library

### antler-pack --add-app
`antler-pack --add-app` will allow the user to add a new app to their project given a current project exists.

Prompts should be given for:
- App name
- App programming language
- App dependencies
   - dependency name
   - dependency location (local or github)
   - dependency version (if location is local then ignore)

With this information a new entry in the `project.yml` will be created.

### antler-pack --add-lib
`antler-pack --add-lib` will allow the user to add a new library to their project given a current project exists.

Prompts should be given for:
- library name
- library programming language
- library dependencies
   - dependency name
   - dependency location (local or github)
   - dependency version (if location is local then ignore)

With this information a new entry in the `project.yml` will be created.

### antler-pack --add-test
`antler-pack --add-test` will allow the user to add a new test to their project given a current project exists.

Prompts should be given for:
- test name
- test dependencies
   - dependency name
   - dependency location (local or github)
   - dependency version (if location is local then ignore)

With this information a new entry in the `project.yml` will be created.

### antler-pack --add-dependency
`antler-pack --add-dependency` will allow the user to add a dependency to their project.

Prompts should be given for:
- Dependency name
- Dependency location (local or github)
- Dependency version (if location is local then ignore)
- App/s names that will use this dependency
- Lib/s names that will use this dependency
- Test/s names that will use this dependency

With this information the associated app/s and lib/s in the `project.yml` will be updated.

### antler-pack --remove-dependency
`antler-pack --remove-dependency` will allow the user to remove a dependency from their project.

Prompts should be given for:
- Dependency name
- App/s to remove the dependency from
- Lib/s to remove the dependency from
- Test/s to remove the dependency from

With this information the associated app/s and lib/s in the `project.yml` will be updated.

A default value for 'project all' should be given for the prompts to allow the dependency to be removed completely.

### antler-pack --update-dependency
`antler-pack --update-dependency` will allow the user to update a given dependency in their project.

Prompts should be given for:
- Dependency name
- New location?
- New version?

With this information the tool will update the associated `project.yml`.

### antler-pack --populate
`antler-pack --populate` should download any dependencies need for the project.

### C++ project.yml parsing library
The following structures and functions should exist.

```C++
namespace antler {

struct version {
   uint16_t major;
   uint16_t minor;
   uint16_t patch;
};

class dependency {
  public:
    // use default constructors, copy and move constructors and assignments
    void update_dependency(version v, std::string&& location) noexcept;
    version get_version() const noexcept;
    std::string_view get_location() const noexcept;
  private:
    std::string name;
    version ver;
    std::string location;
};

class object {
  public:
    // use default constructors, copy and move constructors and assignments

    void upsert_dependency(dependency&& dep) noexcept;
    // returns true if successful, false if the fails (i.e. dependency does not exist)
    bool remove_dependency(std::string_view name) noexcept;

    void update_options(std::string&& options) noexcept;
    void update_language(std::string&& language) noexcept;
    std::string_view get_name() const noexcept;
    std::string_view get_options() const noexcept;
    std::string_view get_language() const noexcept;

    const std::vector<dependency>* get_dependencies() const noexcept;

  private:
    std::string name;
    std::string options;
    std::string language;
    std::unordered_set<dependency> dependencies;
};

// main structure for the project.yml file
class project {
  public:
    // parse a project from a project.yml
    inline project(const char* filename);
    void upsert_app(object&& app) noexcept;
    void upsert_lib(object&& lib) noexcept;
    void upsert_test(object&& test) noexcept;
    const std::unordered_set<object> get_apps() const noexcept;
    const std::unordered_set<object> get_libs() const noexcept;
    const std::unordered_set<object> get_tests() const noexcept;
  private:
    std::string file_location;
    std::string project_name;
    version ver;
    std::unordered_set<object> apps;
    std::unordered_set<object> libs;
    std::unordered_set<object> tests;
};

} // ns antler
```

# Example projects

https://github.com/larryk85/sample_contract
https://github.com/larryk85/test_lib
