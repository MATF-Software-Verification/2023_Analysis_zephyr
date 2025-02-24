#!/bin/bash

ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=~/2023_Analysis_zephyr/zephyr
TEST_DIR=~/2023_Analysis_zephyr/testing/unit/bt_le_cs_security_enable

echo "Integrating test into twister test structure..."
cp -r ${TEST_DIR} ${ZEPHYR_DIR}/tests/bluetooth/host/cs/

echo "Changing pwd to ${ROOT_DIR}..."
cd ${ROOT_DIR}

echo "Running twister for host cs tests..."
twister --coverage -p unit_testing -T zephyr/tests/bluetooth/host/ -v

echo "Pulling test reports into ${TEST_DIR}..."
mv ${ROOT_DIR}/twister-out/ ${TEST_DIR}/

echo "Filtering the coverage reports..."
cd ${TEST_DIR}/twister-out
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
lcov --remove coverage.info "*/tests/*" "*/subsys/testsuite/*" --output-file coverage_filtered.info --rc lcov_branch_coverage=1
genhtml coverage_filtered.info --output-directory coverage_filtered --branch-coverage

firefox ./coverage_filtered/index.html &