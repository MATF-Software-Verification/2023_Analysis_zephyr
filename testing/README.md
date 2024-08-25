# Unit and Integration Testing

This document outlines the structure and procedures for conducting unit and integration testing in the Zephyr project, with a focus on the Bluetooth and cryptography subsystems.

## Testing Directory Structure

The testing directory is organized as follows:

```
├── patches
│   └── bt_crypto.patch
├── testing
│   ├── integration
│   │   └── bt_crypto
|   |       ├── CMakeLists.txt
|   |       ├── prj.conf
|   |       ├── src
|   |       |   └── test_crypto.c
|   |       └── testcase.yaml
│   └── unit
│       ├── bt_encrypt_decrypt
│       └── run_unit_tests.sh
```


### Patches

The `patches` directory contains patches that modify the Zephyr submodule. For example, `bt_crypto.patch` is used to add a test folder within the Zephyr Bluetooth host crypto implementation, which is necessary for the unit tests.

### Unit Testing

The `testing/unit/` directory contains the unit tests for individual functions or components.

- **bt_encrypt_decrypt**: This folder is currently empty because the actual test sources are integrated into the Zephyr submodule using the patch (`bt_crypto.patch`). The tests themselves are designed to validate the encryption and decryption processes in isolation.

- **run_unit_tests.sh**: A script to automate the process of applying the patch, running the unit tests with coverage reporting, and cleaning up afterward.

### Integration Testing

The `testing/integration/` directory is where integration tests are stored. These tests aim to validate the interactions between various components within a subsystem.

- **bt_crypto**: This folder contains tests that validate the Bluetooth cryptographic functions as a whole. The tests are run on the `native_posix_64` platform, which simulates the behavior in a controlled environment rather than on real hardware.

## Zephyr Submodule

The Zephyr submodule is included @ 419dda3ed72 commit. The tests rely on specific patches to add tests that are not be present in the main repository.

## Running Tests

### Unit Tests

To run the unit tests:

Execute `run_unit_tests.sh` script from `unit/` folder which will:

1. Apply the patch to zephyr submodule.
2. Execute the tests using Zephyr's `twister` tool.
3. Generate coverage reports for the tests.
4. Cleanup the zephyr submodule

### Integration Tests

To run integration tests:

1. Ensure that the necessary patches are applied to the Zephyr submodule.
2. Use `twister` to execute the tests on the `native_posix_64` platform.

### Notes

- The integration tests do not represent a real-world scenario since they are run on a simulated environment (`native_posix_64`).
- The unit tests are tightly integrated with the Zephyr source code via patches, so the sources are not present directly within the `testing/unit/` directory.

## Additional Considerations

If you plan to include a test in the appropriate folder within the submodule, note that it might not be built by default unless the patch is applied. This setup allows flexibility in managing test code that is closely related to the Zephyr codebase.

