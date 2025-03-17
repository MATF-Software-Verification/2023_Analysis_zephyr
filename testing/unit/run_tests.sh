#!/bin/bash

# Configuration
ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=${ROOT_DIR}/zephyr
TEST_BASE_DIR=${ROOT_DIR}/testing/unit

# Available test directories
TEST_DIRS=("bt_le_cs_security_enable" "channel_sounding" "channel_sounding_behavior")

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

ZEPHYR_TARGET_TEST_DIR=${ZEPHYR_DIR}/tests/bluetooth/host/cs/${TEST_FOLDER}

cp -r ${TEST_DIR} ${ZEPHYR_DIR}/tests/bluetooth/host/cs/ || {
    log "Error: Failed to copy test folder to Zephyr directory."
    exit 1
}

# Step 2: Change to the root directory
log "Changing pwd to ${ROOT_DIR}..."
cd ${ROOT_DIR} || {
    log "Error: Failed to change directory to ${ROOT_DIR}."
    exit 1
}

# Step 3: Run twister for host CS tests
log "Running twister..."
if [ "$COVERAGE" = true ]; then
    log "with full coverage"
    twister --coverage -p unit_testing -T zephyr/tests/bluetooth/host/ -v || {
        log "Error: Twister failed to run."
        exit 1
    }
else
    #TODO:  - 'Focused' option - run exactly what was selected
    #       - 'Full' option - run all of the tests in 'base' dir (e.g. <full_path>/host)
    log "focused - no coverage"
    log "CMD: twister -p unit_testing -T ${ZEPHYR_TARGET_TEST_DIR} -v"
    twister -p unit_testing -T ${ZEPHYR_TARGET_TEST_DIR} -v || {
        log "Error: Twister failed to run."
        exit 1
    }
fi

# Step 4: Pull test reports into the test directory
log "Pulling test reports into ${TEST_DIR}..."
mv ${ROOT_DIR}/twister-out/ ${TEST_DIR}/ || {
    log "Error: Failed to move twister-out to ${TEST_DIR}."
    exit 1
}

# Step 5: Filter the coverage reports (if coverage is enabled)
if [ "$COVERAGE" = true ]; then
    log "Filtering the coverage reports..."
    cd ${TEST_DIR}/twister-out || {
        log "Error: Failed to change directory to ${TEST_DIR}/twister-out."
        exit 1
    }

    lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1 || {
        log "Error: Failed to capture coverage data."
        exit 1
    }

    lcov --remove coverage.info "*/tests/*" "*/subsys/testsuite/*" --output-file coverage_filtered.info --rc lcov_branch_coverage=1 || {
        log "Error: Failed to filter coverage data."
        exit 1
    }

    genhtml coverage_filtered.info --output-directory coverage_filtered --branch-coverage || {
        log "Error: Failed to generate HTML coverage report."
        exit 1
    }

    # Step 6: Open the coverage report in the default web browser
    log "Opening the coverage report in the default web browser..."
    xdg-open ./coverage_filtered/index.html &
fi

log "Script completed successfully."