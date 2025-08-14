#!/bin/bash

APP_DIR="./zephyr/samples/bluetooth/beacon"
BUILD_DIR="build_tidy"

TARGET_FILE="./zephyr/subsys/bluetooth/host/cs.c"

LOG_FILE="tidy_report.log"

CHECKS='bugprone-*,performance-*,readability-*'

echo "Building the project to generate compile_commands.json..."

west build -b native_sim ${APP_DIR} -d ${BUILD_DIR} -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if [ ! -f "${BUILD_DIR}/compile_commands.json" ]; then
    echo "Build failed or compile_commands.json was not created. Aborting."
    exit 1
fi

echo "Compile commands are ready."

echo "Running clang-tidy on ${TARGET_FILE}..."

clang-tidy -p ${BUILD_DIR} --checks="${CHECKS}" ${TARGET_FILE} 2>&1 | grep "${TARGET_FILE}" > ${LOG_FILE}

# Check if clang-tidy produced any output (warnings)
if [ -s "${LOG_FILE}" ]; then
    echo "clang-tidy found potential issues."
    echo "   See the full report in '${LOG_FILE}'."
else
    echo "No issues found by clang-tidy for the selected checks."
fi