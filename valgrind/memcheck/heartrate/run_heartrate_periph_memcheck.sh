#!/bin/bash

usage() {
	echo "Usage $0 [-h] [d HCI_DEVICE] [-t TIMEOUT]"
	echo " -d HCI_DEVICE Specify the BLE capable hci device. Default: hci0"
	echo " -t TIMEOUT Specify the timeout in seconds. Default: 0 (no timeout)"
	echo " -h Show help message"
	exit 1
}

# Setup the environment
PROJECT_BASE=$HOME/2023_Analysis_zephyr/

source $PROJECT_BASE/zephyr/zephyr-env.sh

#default values
HCI_DEVICE="hci0"
TIMEOUT=0
MAX_RETRIES=3
SUPPRESSION_FILE=$PROJECT_BASE/zephyr/scripts/valgrind.supp

ZEPHYR_APP="./build/zephyr/zephyr.exe"

valgrind_output_dir=$PROJECT_BASE/valgrind/memcheck/heartrate

mkdir -p $valgrind_output_dir

while getopts "d:t:h" opt; do
	case $opt in
		d) HCI_DEVICE="$OPTARG" ;;
		t) TIMEOUT="$OPTARG" ;;
		h) usage ;;
		*) echo "Invalid option: -$OPTARG" >&2; usage ;;
	esac
done

if ! [[ "$TIMEOUT" =~ ^[0-9]+$ ]]; then
	echo "Error: Please enter non-negative int"
	exit 1
fi

cleanup(){
	echo "Caught SIGINT. Cleaning up..."
	kill $APP_PID 2>/dev/null
	kill $TIMER_PID 2>/dev/null
	pkill -P $$ 2>/dev/null
	exit 1
}

trap cleanup SIGINT

# Bring down hci to avoid EBUSY errors
bring_down_hci() {
	echo "Bringing down HCI interface: $HCI_DEVICE"

	if sudo hciconfig $HCI_DEVICE | grep -q "UP RUNNING"; then
		sudo hciconfig $HCI_DEVICE down
		if [ $? -ne 0 ]; then
			echo "Failed to bring down HCI interface. Exiting."
			return -1
		fi
	else
		echo "$HCI_DEVICE already down"
		return 0
	fi
}

build_app() {
	echo "Building the app..."
	west build -p auto -b native_posix $ZEPHYR_BASE/samples/bluetooth/peripheral_hr/ -- -DOVERLAY_CONFIG=$ZEPHYR_BASE/samples/bluetooth/peripheral_hr/overlay-extended.conf
}

run_valgrind() {
	echo "Running zephyr application with Valgrind"
	if [ $TIMEOUT -gt 0 ]; then
		echo "Timeout set to $TIMEOUT seconds."
		echo "Using suppressions: --suppressions=$SUPPRESSION_FILE"
		sudo valgrind --tool=memcheck --suppressions=$SUPPRESSION_FILE --show-leak-kinds=all --leak-check=full --track-origins=yes --log-file="$valgrind_output_dir/valgrind_$(date +%Y%m%d_%H%M%S).log" $ZEPHYR_APP --bt-dev=$HCI_DEVICE & APP_PID=$!

		(
			sleep $TIMEOUT
			echo "Timeout reached. Killing the app..."
			if ps -p $APP_PID > /dev/null; then
        		kill -9 $APP_PID 2>/dev/null
    		fi
		) &
		TIMER_PID=$!

		wait $APP_PID
		APP_EXIT_CODE=$?

		kill $TIMER_PID 2>/dev/null

		if [ $APP_EXIT_CODE -eq 137 ]; then
			echo "Zephyr app was killed after $TIMEOUT seconds."
			return 124
		else
			return $APP_EXIT_CODE
		fi
	else
		echo "Using suppressions: --suppressions=$SUPPRESSION_FILE"
		sudo valgrind --tool=memcheck --suppressions=$SUPPRESSION_FILE --show-leak-kinds=all --leak-check=full --track-origins=yes --log-file="$valgrind_output_dir/valgrind_$(date +%Y%m%d_%H%M%S).log" $ZEPHYR_APP --bt-dev=$HCI_DEVICE
	fi
	return $?
}

build_app

retry_count=0
while [ $retry_count -lt $MAX_RETRIES ]; do	
	bring_down_hci

	if [ $? -ne 0 ]; then
		echo "Retrying... ($retry_count/$MAX_RETRIES)"
		echo "Failed to bring down hci after $MAX_RETRIES retries. Exiting"
		exit 1
	fi
	retry_count=$((retry_count +1))
done

run_valgrind
exit_code=$?

if [ $exit_code -eq 0 ]; then
	echo "Zephyr app completed succesfully"
	exit 0
elif [ $exit_code -eq 124 ]; then
	echo "Zephyr app timed out after $TIMEOUT seconds."
else
	echo "Zephyr app failed with exit code $exit_code"
fi

stty sane