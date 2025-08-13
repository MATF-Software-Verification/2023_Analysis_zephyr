# Overview of the implemented patches

This document outlines the purpose and background for each of the patches in the current folder.

1) `cs_error_handling.patch`
    * Purpose: Fix handling of the invalid inputs in the Channel Sounding (cs) module.
    * During the unit testing of the aforementioned module, a couple of errors were uncovered due to invalid input handling (e.g. not checking the NULL values).
    * This patch adds inputs and return values checks in order to avoid errors.

2) `userchan_fix_uninit_access.patch`
    * Purpose: Fix the usage of memory errors reported by Valgrind memcheck.
    * During the `bind()` system call in `userchan.c`, Valgrind detected the uninitialized bytes usage. The problem here was that the `sockaddr_hci` struct (that was in use) and `sockaddr` struct (that `bind` expects) are not matching in size and layout, which caused the `bind` to access memory outside of the passed structure.
    * This patch adds the initialization of a bigger structure, `sockaddr`, fills in the relevant fields, and only then pass it to `bind`.

3) `nsi_cleanup.patch`
    * Purpose: Eliminate the false positive memory leaks reported by Valgrind on `nrf52_bsim` platform.
    * Valgrind used to report still reachable and possibly lost memory leaks within the `nsi_tasks.c`. The functions for freeing up the memory haven't been called due to the nature of simulated execution - `ON_EXIT` tasks are never executed because the program is running in an endless loop.
    * This patch adds `nsi_exit(0)` at the end of `main` in order to avoid the leakage. Additionally, the patch adds a maximal number of disconnects to the advertiser app that was tested.

4) Demo patches for demonstration of the Valgrind tool
    * This group of patches is not intended for bug fixing but serves to demonstrate Valgrind's capability to detect various types of memory errors.
    * `memleak_demo_initial.patch`: Introduces `mem_leak()` and `use_uninitialized_mem()` functions into the beacon sample to show basic memory error detection.
    * `memleak_demo_no_optimize.patch`: Adds `CONFIG_NO_OPTIMIZATIONS=y` to `prj.conf`. This prevents the compiler from removing the unused functions containing the intentional bugs, ensuring Valgrind can analyze them.
    * `memleak_demo_all_leaks.patch`: Expands the sample with functions that demonstrate all major memory leak categories (indirectly lost, possibly lost, still reachable).