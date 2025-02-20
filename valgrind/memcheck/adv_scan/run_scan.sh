#!/bin/bash

cd $BSIM_OUT_PATH/bin/

PROJECT_BASE=$HOME/2023_Analysis_zephyr/

SUPPRESSION_FILE=$PROJECT_BASE/zephyr/scripts/valgrind.supp

valgrind_output_dir_scan=$PROJECT_BASE/valgrind/memcheck/adv_scan/memcheck_scanner

mkdir -p $valgrind_output_dir_scan

echo "Running valgrind on scanner app, and logging into $valgrind_output_dir_scan"

sudo valgrind --tool=memcheck --suppressions=$SUPPRESSION_FILE \
    --leak-check=full --show-leak-kinds=all --track-origins=yes \
    --log-file="$valgrind_output_dir_scan/valgrind_$(date +%Y%m%d_%H%M%S).log" \
    ./scanner.exe -s=extended_adv_valgrind_test -d=1 & APP_PID=$!


sleep 45

sudo kill -9 $APP_PID