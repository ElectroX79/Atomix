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
git clone 
cd Atomix
```

### Then you two options: 

___

## 1. Run the demos
**If you just want to run the demos and try the current API's:**

Demos are located in the `demo` directory, you can build and execute them following
the steps indicated in the readme located in `demo/scripts/README.md`.

In `demo/scripts/README.md` you will find three bash scripts and a README.md, you should
follow the instructions in the README.md to run the scripts.

___

## 2. Build the project

Create a build directory with CMake:

```bash
cmake -B build -G Ninja
```

Then build the project:

```bash
cmake --build build -G Ninja
```

### 2.1 Run the tests
**If you want to run the tests**:

After building the project, run the test suite:

```bash
cmake --build build --target <test-target>
./build/<test-target>
```

### 2.2 Run a benchmark test
**To run a benchmark test:**

Benchmarks can be built and run like other targets. The code itself is usually located in `test/benchmark`.

For example:

```bash
cmake --build build --target <benchmark-target>
./build/<benchmark-target>
```

Some benchmarks use additional tools such as `perf` which could be automated entirely with a bash script. Check the [benchmark](../contributing/benchmark.md) documentation before running them.

___

## Next steps

Once you are able to build and test Atomix, you can:

### Learn more about the project

- [Architecture](architecture/atomix_arquitecture.md): To understand how Atomix is structured and how it works.
- [Internals](internals/introduction.md): Where you can find information about the internal design of Atomix.


### Consider contributing to the project:
- [Contributing](../contributing/introduction.md): To learn how to contribute to the project.

### Explore potential future features:
- [Future features](../future_features/introduction.md): To understand the plans for the project.


