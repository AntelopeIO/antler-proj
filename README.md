| :memo:        | As part of our ongoing commitment to efficiently deliver value to the EOS Network, Antler has been deprecated as of January 2024. This repository will not receive future updates or support. Your questions and concerns are important to us; please direct feedback to our [Antler Deprecation Feedback Tracker](https://github.com/eosnetworkfoundation/product/issues/282) |
|---------------|:------------------------|

# ANTLER Project Tools

ANTLER Project Tools (aproj) is a set of tools to help in the
generation of smart contracts intended for deployment to an
[Antelope](https://github.com/AntelopeIO/) blockchain.

This set of tools will help the developer create, maintain, build, and
test smart contracts.


## Usage

For now, please see the [Intial Design Document](./docs/project_manager.md) for usage.

## Runtime Dependencies

The following tools are required for aproj to work at runtime:
- CMake v3.11


## Build Dependencies

Building aproj requires the following dependencies:
- C++20 compiler (common minimums: gcc-10 or clang-11).
- [CMake](https://cmake.org/overview/) version 3.11 or greater (note the requirement [here](./CMakeLists.txt#L1)).

Additionally we recomend using [ninja build](https://ninja-build.org/) for fast builds.

## Submodules

Make sure your submodules are initialized and updated recursively using the command:
`git submodule update --init --recursive`

## Building

Assuming you have the other dependencies installed, from the project root, the following will build the project:
```bash
mkdir -p Build \
    && cd Build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && cmake --build .
```

## Test

Currently, the following will run some automated tests:
```bash
ninja && ninja test
```

## Maintaining

Directories in the project root:
- aproj: This contains the various CLI tools, all of which depend on the project library.
- project: This is standalone library that contains the majority of implementation that creates and maintains the `project.yaml` file as well as initialize and populate the project's directory tree, among other things.
- test: This direcory contains a number of unit tests. In future, it's likely to contain system level tests as well.


## License

This project is licensed using the MIT License as found
[here](./LICENSE).

Additionally, apack includes and depends on these libraries:
- [Rapid YAML](https://github.com/biojppm/rapidyaml) - `MIT`.
- [CLI11](https://github.com/CLIUtils/CLI11) - `BSD-3-Clause`.
- [MagicEnum](https://github.com/Neargye/magic_enum) - `MIT`.


## Miscellaneous

### What does `aproj` stand for?

It's a shortening of `antler-proj`.

### Is ANTLER an acronym?

Yes, it is. Or maybe it's a
[backronym](https://en.wikipedia.org/wiki/Backronym)?

Regardless, it's ANother TransLator Environment and Runtime.

### Initial Issue

https://github.com/AntelopeIO/cdt/issues/62
