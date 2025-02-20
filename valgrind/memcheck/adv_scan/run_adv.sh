#!/bin/bash

cd $BSIM_OUT_PATH/bin/

PROJECT_BASE=$HOME/2023_Analysis_zephyr

SUPPRESSION_FILE=$PROJECT_BASE/zephyr/scripts/valgrind.supp

valgrind_output_dir_adv=$PROJECT_BASE/valgrind/memcheck/adv_scan/memcheck_advertiser

mkdir -p "$valgrind_output_dir_adv"

echo "Running valgrind on advertiser app, and logging into $valgrind_output_dir_adv"

echo "${valgrind_output_dir_adv}/valgrind_$(date +%Y%m%d_%H%M%S).log"

if [[ $1 != "patched" ]]; then
        # Attach advertiser and simulator to the emulated PHY
        sudo valgrind --tool=memcheck --suppressions="$SUPPRESSION_FILE" \
        --leak-check=full --show-leak-kinds=all --track-origins=yes \
        --log-file="${valgrind_output_dir_adv}/valgrind_$(date +%Y%m%d_%H%M%S).log" \
        ./advertiser.exe -s=extended_adv_valgrind_test -d=0 & APP_PID=$!

        sleep 45
        sudo kill -9 $APP_PID
else
      # Attach advertiser and simulator to the emulated PHY - without sleep
        sudo valgrind --tool=memcheck --suppressions="$SUPPRESSION_FILE" \
        --leak-check=full --show-leak-kinds=all --track-origins=yes \
        --log-file="${valgrind_output_dir_adv}/valgrind_$(date +%Y%m%d_%H%M%S).log" \
        ./advertiser.exe -s=extended_adv_valgrind_test -d=0
fi