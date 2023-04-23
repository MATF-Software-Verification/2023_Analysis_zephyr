# Unit testing

Since the project is huge, the focus will be only on couple of components.

## Zephyr's twister tool

Twister is a useful tool provided by Zephyr for easier testing and coverage reports generation. Documentation regarding Twister can be found [here](https://docs.zephyrproject.org/3.1.0/develop/test/twister.html).

This directory contains folders containins .html coverage reports in lcov format for a couple of different test runs. In order to get output binaries and logs, twister has to be run again. These files can be large, so they are excluded from the project's repo.

Commands that were used for generation of those reports are given below:

- Execute unit tests related to math_extras and store reports at `/test_coverage/unit_run_math_extras` and `/test_coverage/unit_run_math_extras_builtin`. In order to run these tests, a `CMakeLists.txt` had to be modified before running the commands. There was also a naming bug in the `portable.c` file:

```
// portable.c

...

#define VNAME(N) test_portable_##N
// Changed to
#define VNAME(N) run_portable_##N

...

ZTEST(math_extras_portable, test_portable_u32_add) {
	run_portable_u32_add();
}

...

```

Twister command for running the tests:

`west twister --coverage -p unit_testing -T tests/unit/math_extras/ -v --report-dir ~/2023_Analysis_zephyr/twister_reports/test_coverage/unit_run_math_extras --outdir ~/2023_Analysis_zephyr/twister_reports/test_coverage/math_extras/unit_run_math_extras`

*NOTE*: As mentioned, `CMakeLists.txt` was modified between two runs. This was done to test math_extras functions with and without builtin support. 

`CMakeLists.txt` **with** builtin support: 

```
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

project(math_extras)
find_package(Zephyr COMPONENTS unittest REQUIRED HINTS $ENV{ZEPHYR_BASE})
target_sources(testbinary PRIVATE main.c)
```

`CMakeLists.txt` **without** builtin support:

```
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

project(math_extras)
find_package(Zephyr COMPONENTS unittest REQUIRED HINTS $ENV{ZEPHYR_BASE})
target_sources(testbinary PRIVATE portable.c)
```

The only difference is the target source file. 

### Ztest framework

In the section above, tests were run without any modification or explanation. Zephyr project has developed a testing framework, `Ztest` that alows writing and executing various tests. Comprehensive documentation for Ztest is given [here](https://docs.zephyrproject.org/3.1.0/develop/test/ztest.html). Source code for math_extras tests is in `tests/unit/math_extras/main.c`, `tests/unit/math_extras/portable.c` and `tests/unit/math_extras/tests.inc`.

`main.c` and `portable.c` contain the main **test** logic. In other words, test cases are defined in those files. Helper functions that call functions under test are defined in `tests.inc` in the form of `run_<function_name>` / `run_portable_<function_name>` based on the macro `VNAME(x)` defined in both `main.c` and `portable.c`. Twister will build and execute the tests based on the source files listed in `CMakeLists.txt` (`target_sources` line).

Each test case is defined using `ZTEST` macro with arguments defining test suite name and test case name. These will be visible during test run. 

For example, test case defined as: 
```
ZTEST(math_extras, test_u32_add) {
	run_u32_add();
}
```

means that Twister will call `math_extras.test_u32_add` test case during execution. The body of the `ZTEST`-defined test case contains all of the test logic (setting up the test, executing helper functions, functions under test, deinitialization etc.)
 
### lcov report

lcov is a tool that creates graphical representation of the code coverage in .html format. Reports for math_extras functions are given below:

![code coverage report](./test_coverage/mat_extras/lcov_math_extras_report.png "lcov report").


As the reports show, there is a 100% coverage of the `math_extras` functions both with builtin functions and with portable implementation.
