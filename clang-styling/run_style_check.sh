#!/bin/bash

PROJECT_BASE="${HOME}/2023_Analysis_zephyr"
ZEPHYR_DIR="${PROJECT_BASE}/zephyr"

TARGET_DIR="${ZEPHYR_DIR}/subsys/bluetooth/host/"

OUTPUT_LOG="${PROJECT_BASE}/clang-format/bt_host_clang-format.log"

STYLE="file"

FIX_MODE=false
if [[ "$1" == "--fix" ]]; then
    FIX_MODE=true
    echo "Running in FIX mode - modifying files in place."
else
    echo "Running in CHECK mode. Use '--fix' to apply changes."
fi

FILES=$(find ${TARGET_DIR} -name "*.c" -o -name "*.h")

echo "Checking the following files:"
echo "${FILES}"
echo "============================="

if [[ "$FIX_MODE" = true ]]; then
    clang-format -i -style=${STYLE} ${FILES}
    echo "Formatting done"
else
    clang-format --dry-run -Werror -style=${STYLE} ${FILES} 2> ${OUTPUT_LOG}
    if [ $? -eq 0 ]; then
        echo "All files formatted correctly."
    else
        echo "Formatting issues found, check the ${OUTPUT_LOG} for details."
        exit 1
    fi
fi