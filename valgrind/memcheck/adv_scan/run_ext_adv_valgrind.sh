#!/bin/bash

PROJECT_BASE=$HOME/2023_Analysis_zephyr/

source ${PROJECT_BASE}zephyr/zephyr-env.sh

# Source the babblesim paths
source $HOME/.bashrc

# TODO(avra): Add option for applying the patch
ADV_ARGS=""
if [[ $1 == "patched" ]]; then
    ADV_ARGS="patched"
fi

# Build for babblesim board

# Build and rename advertiser executable
west build -p auto -b nrf52_bsim $ZEPHYR_BASE/samples/bluetooth/extended_adv/advertiser
mv ./build/zephyr/zephyr.exe $BSIM_OUT_PATH/bin/advertiser.exe

# # Build and rename scanner executable
west build -p auto -b nrf52_bsim $ZEPHYR_BASE/samples/bluetooth/extended_adv/scanner
mv ./build/zephyr/zephyr.exe $BSIM_OUT_PATH/bin/scanner.exe

gnome-terminal -- bash -c ./run_babblesim.sh; exec bash &

sleep 10

valgrind_output_dir_adv=memcheck_advertiser
valgrind_output_dir_scan=memcheck_scanner

mkdir -p "$valgrind_output_dir_adv"
mkdir -p "$valgrind_output_dir_scan"

gnome-terminal -- bash -c ./run_adv.sh ${ADV_ARGS}; exec bash &

sleep 10

# # Attach advertiser and simulator to the emulated PHY
# sudo valgrind --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir_adv/valgrind_$(date +%Y%m%d_%H%M%S).log" \
#         ./advertiser.exe -s=extended_adv_valgrind_test -d=0 &

# sudo valgrind --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir_scan/valgrind_$(date +%Y%m%d_%H%M%S).log" \
#         ./scanner.exe -s=extended_adv_valgrind_test -d=1 &

gnome-terminal -- bash -c ./run_scan.sh; exec bash &

sleep 10

# Kill all the running processes after 10 seconds
# sudo killall -9 advertiser.exe scanner.exe bs_2g4_phy_v1

# Cleanup
rm -rf ./build