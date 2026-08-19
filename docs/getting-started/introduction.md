# Getting Started

This guide explains how to get Atomix, build the project and run its tests.

## Requirements

Before building Atomix, make sure you have:

* Git
* CMake
* A C++ compiler with C++23 support

Atomix is currently mainly developed and tested on Linux.

## Clone the repository

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd Atomix
```

## Build the project

Create a build directory with CMake:

```bash
cmake -B build
```

Then build the project:

```bash
cmake --build build
```

## Run the tests

After building the project, run the test suite:

```bash
cmake --build build --target <test-target>
./build/<test-target>
```

## Run a benchmark test

Benchmarks can be built and run like other targets. The code itself is usually located in `test/benchmark`.

For example:

```bash
cmake --build build --target <benchmark-target>
./build/<benchmark-target>
```

Some benchmarks use additional tools such as `perf` which could be automated entirely with a bash script. Check the [benchmark](../contributing/benchmark.md) documentation before running them.

## Next steps

Once you are able to build and test Atomix, you can:

### Learn more about the project

- [Architecture](architecture/introduction.md): To understand how Atomix is structured and how it works.
- [Internals](internals/introduction.md): Where you can find information about the internal design of Atomix.


### Consider contributing to the project:
- [Contributing](../contributing/introduction.md): To learn how to contribute to the project.

### Explore potential future features:
- [Future features](../future_features/introduction.md): To understand the plans for the project.


