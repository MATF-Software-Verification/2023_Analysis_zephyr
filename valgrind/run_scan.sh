#!/bin/bash

cd $BSIM_OUT_PATH/bin/

sudo valgrind --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir_scan/valgrind_$(date +%Y%m%d_%H%M%S).log" \
        ./scanner.exe -s=extended_adv_valgrind_test -d=1


sleep 15

sudo killall -9 ./scanner.exe