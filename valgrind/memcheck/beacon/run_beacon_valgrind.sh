#!/bin/bash

usage() {
	echo "Usage $0 [-h] [d HCI_DEVICE] [-t TIMEOUT]"
	echo " -d HCI_DEVICE Specify the BLE capable hci device. Default: hci0"
	echo " -t TIMEOUT Specify the timeout in seconds. Default: 0 (no timeout)"
    echo " -c y|n Specify if cleanup is needed. Default: n (no cleanup)"
    echo " -s y|n Specify whether the suppression file for valgrind is to be used"
	echo " -h Show help message"
	exit 1
}

PROJECT_BASE=$HOME/2023_Analysis_zephyr/

source $PROJECT_BASE/zephyr/zephyr-env.sh

HCI_DEVICE=hci0
CLEAN="n"
TIMEOUT=0
delay=5
SUPPRESSION="n"
SUPPRESSION_FILE=$PROJECT_BASE/zephyr/scripts/valgrind.supp
SUPPRESSION_FLAG=""

while getopts "d:t:c:s:h" opt; do
	case $opt in
		d) HCI_DEVICE="$OPTARG" ;;
		t) TIMEOUT="$OPTARG" ;;
        c) CLEAN="$OPTARG" ;;
        s) SUPPRESSION="$OPTARG" ;;
		h) usage ;;
		*) echo "Invalid option: -$OPTARG" >&2; usage ;;
	esac
done

# build the application
west build -p auto -b native_posix $ZEPHYR_BASE/samples/bluetooth/beacon

# Bring down hci to avoid EBUSY errors
bring_down_hci() {
	echo "Bringing down HCI interface: $HCI_DEVICE"
	sudo hciconfig $HCI_DEVICE down
	if [ $? -ne 0 ]; then
		echo "Failed to bring down HCI interface. Exiting."
		exit 1
	fi
}

valgrind_output_dir=.

bring_down_hci

if [ "$SUPPRESSION" == "y" ]; then
    SUPPRESSION_FLAG="--suppressions=$SUPPRESSION_FILE"
    echo "Using supression - $SUPPRESSION_FLAG"
fi

sudo valgrind $SUPPRESSION_FLAG --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir/valgrind_$(date +%Y%m%d_%H%M%S).log" --show-leak-kinds=all --track-origins=yes ./build/zephyr/zephyr.exe --bt-dev="$HCI_DEVICE" > "$valgrind_output_dir/valgrind_stdout_$(date +%Y%m%d_%H%M%S).log" 2>&1 &
VALGRIND_PID=$!
sleep $TIMEOUT

sudo kill $VALGRIND_PID

if [ "$CLEAN" == "y" ]; then
    echo "Cleaning up ..."
    rm -rf ./build
fi

reset