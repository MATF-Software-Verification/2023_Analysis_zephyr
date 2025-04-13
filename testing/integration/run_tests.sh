#!/bin/bash

# Configuration
ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=${ROOT_DIR}/zephyr
TEST_BASE_DIR=${ROOT_DIR}/testing/integration

# Available test directories
TEST_DIRS=("psa_hci_integration" "rpa_psa_integration")

# Function to display usage
usage() {
    echo "Usage: $0 <test_folder> [--no-coverage]"
    echo "Available test folders:"
    for dir in "${TEST_DIRS[@]}"; do
        echo "  - ${dir}"
    done
    echo "Options:"
    echo "  --no-coverage  Run tests without generating coverage reports."
    exit 1
}

# Check if a test folder is provided
if [ $# -lt 1 ]; then
    usage
fi

# Validate the test folder
TEST_FOLDER=$1
if [[ ! " ${TEST_DIRS[@]} " =~ " ${TEST_FOLDER} " ]]; then
    echo "Error: Invalid test folder '${TEST_FOLDER}'."
    usage
fi

# Check for the --no-coverage option
COVERAGE=true
if [ $# -ge 2 ] && [ "$2" == "--no-coverage" ]; then
    COVERAGE=false
fi

# Set the full path to the test directory
TEST_DIR=${TEST_BASE_DIR}/${TEST_FOLDER}

# Logging function
log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $1"
}

# Source the zephyr environment
log "Setting up the zephyr environment..."
source ${ZEPHYR_DIR}/zephyr-env.sh

# Step 1: Integrate the test into the Zephyr test structure
log "Integrating test into twister test structure..."

log "Making dir: ${ZEPHYR_DIR}/tests/integration_tests/..."
mkdir -p ${ZEPHYR_DIR}/tests/integration_tests/

ZEPHYR_TARGET_TEST_DIR=${ZEPHYR_DIR}/tests/integration_tests/${TEST_FOLDER}

rm -rf ${ZEPHYR_TARGET_TEST_DIR}/*

cp -r ${TEST_DIR} ${ZEPHYR_TARGET_TEST_DIR} || {
    log "Error: Failed to copy test folder to Zephyr directory."
    exit 1
}

# Step 2: Change to the root directory
log "Changing pwd to ${ROOT_DIR}..."
cd ${ROOT_DIR} || {
    log "Error: Failed to change directory to ${ROOT_DIR}."
    exit 1
}

# Step 3: Run twister for integration tests
log "Running twister..."
if [ "$COVERAGE" = true ]; then
    log "with full coverage"
    twister --coverage -p native_sim -T ${ZEPHYR_TARGET_TEST_DIR} -v || {
        log "Error: Twister failed to run."
        exit 1
    }
else
    #TODO:  - 'Focused' option - run exactly what was selected
    #       - 'Full' option - run all of the tests in 'base' dir (e.g. <full_path>/host)
    log "focused - no coverage"
    log "CMD: twister --integration -p native_sim -T ${ZEPHYR_TARGET_TEST_DIR} -v --device-testing" # --device-testing
    # twister --integration -p native_sim -T ${ZEPHYR_TARGET_TEST_DIR} -v -- -bt-dev=hci2
    # twister --build-only -p native_sim -T ${ZEPHYR_TARGET_TEST_DIR} -v || { # --device-testing
    twister --integration -p native_sim -T ${ZEPHYR_TARGET_TEST_DIR} -v || {
        log "Error: Twister failed to run."
        exit 1
    }
    # sudo /home/aleksandar/2023_Analysis_zephyr/twister-out/native_sim_native/tests/integration_tests/rpa_psa_integration/rpa_psa_integration/sample.testing.rpa/zephyr/zephyr.exe --bt-dev=hci2
fi