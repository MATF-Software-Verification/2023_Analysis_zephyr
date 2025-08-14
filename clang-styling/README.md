# Overview

This directory contains scripts and the results of statical analysis tools applied to Zephyr OS's bluetooth host submodule. The goal of the analysis was to check this module's compliance with the `.clang-format` rules in the root of `zephyr/` directory and to uncover the other possible issues with the code.

## Style check

`run_style_check.sh` script is used to automatize `clang-format` runs as follows:

* All of the .c and .h files are found within the target directory (`subsys/bluetooth/host`).
* `clang-format` is run in one of the following two modes:
    * Check mode: runs in `--dry-run` mode, only reporting the errors without fixing them. Violations are stored in a `.log` file.
    * Fix mode: runs with an `-i` option, fixing the violations in place.

### Results

There were no major violations reported, all of the 13k reported lines point towards the minor styling changes. Those were applied, and the the resulting changes are stored in a `host_clang_suggestions.diff` file.

## Clang-tidy analysis

The focus of the clang-tidy analysis is on the subsys/bluetooth/host/cs.c file. The goal is to identify possible errors, dangerous (bug-prone) coding patterns, and some optimization possibilities.

`run_tidy_check.sh` script is used to automatize `clang-tidy` runs. The script builds a sample application to generate a `compile_commands.json` file, which is necessary for the analysis, and then runs `clang-tidy`, filtering the output to show only warnings relevant to the target file (`cs.c`).

### Results

Analyzing the `cs.c` file, the `clang` tool found several issues with the code. Although no errors were found that would cause a system crash, warnings point towards the problems with maintainability, readability and potential bugs.

Below are the most important categories of issues found:

1) High cognitive complexity (`readability-function-cognitive-complexity`)
    Complexity is calculated incrementally by noticing confusing patterns within a function, such as nested loops, conditional statements, logical operators, and jumps that disrupt the linear flow of the code.
    * `alloc_reassembly_buf`: 60
    * `bt_hci_le_cs_read_remote_supported_capabilities_complete`: 128
    * `bt_hci_le_cs_read_remote_fae_table_complete`: 107
    Given that the threshold was set at 25, these functions are extremely difficult to understand, debug and modify. This can be considered the most serious risk found by the analysis.

2) Potential bugs
    * `bugprone-narrowing-conversions`
        `clang-tidy` detected "narrowing conversions," where a value of a wider type (e.g., `unsigned long`) is assigned to a variable of a narrower, signed type (`int16_t`).
        These types of conversions can lead to data loss or unexpected behavior if the original value cannot fit into the target type, which is a potential source of bugs.
    * `bugprone-easily-swappable-parameters`
        The function `bt_le_cs_get_antenna_path` has two adjacent parameters of the same type (`uint8_t`). It's possible (and plausible) to mistakenly swap these parameters when calling the function, and the compiler would not report an error, making this bug extremely hard to find.

3) Readability issues
    * `readability-magic-numbers`
    The code uses "magic numbers" (e.g., 12, 10, 0xFF) without a clear name or context.
    * `readability-identifier-length`
    The use of overly short variable names such as i, q, and cp. `clang-tidy` suggests that longer, more descriptive names improve the overall readability of the code.

Full report is stored within the `tidy_report.log` file.
