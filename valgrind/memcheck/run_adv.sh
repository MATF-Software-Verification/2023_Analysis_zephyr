#!/bin/bash

cd $BSIM_OUT_PATH/bin/

# Attach advertiser and simulator to the emulated PHY
sudo valgrind --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir_adv/valgrind_$(date +%Y%m%d_%H%M%S).log" \
        ./advertiser.exe -s=extended_adv_valgrind_test -d=0


sleep 25

sudo killall -9 ./advertiser.exe