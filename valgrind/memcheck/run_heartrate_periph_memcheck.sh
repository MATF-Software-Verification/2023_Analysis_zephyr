#!/bin/bash

usage() {
	echo "Usage $0 [-h] [d HCI_DEVICE] [-t TIMEOUT]"
	echo " -d HCI_DEVICE Specify the BLE capable hci device. Default: hci0"
	echo " -t TIMEOUT Specify the timeout in seconds. Default: 0 (no timeout)"
	echo " -h Show help message"
	exit 1
}

#default values
HCI_DEVICE="hci0"
TIMEOUT=0
MAX_RETRIES=3

ZEPHYR_APP="./build/zephyr/zephyr.exe"

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
	sudo hciconfig $HCI_DEVICE down
	if [ $? -ne 0]; then
		echo "Failed to bring down HCI interface. Exiting."
		exit 1
	fi
}

run_valgrind() {
	echo "Running zephyr application with Valgrind"
	if [ $TIMEOUT -gt 0 ]; then
		echo "Timeout set to $TIMEOUT seconds."
		sudo valgrind --leak-check=full --track-origins=yes $ZEPHYR_APP --bt-dev=$HCI_DEVICE & APP_PID=$!

		(
			sleep $TIMEOUT
			echo "Timeout reached. Killing the app..."
			kill $APP_PID 2>/dev/null
		) &
		TIMER_PID=$!

		wait $APP_PID
		APP_EXIT_CODE=$?

		kill $TIMER_PID 2>/dev/null

		if [ $APP_EXIT_CODE -eq 142 ]; then
			echo "Zephyr app was killed after $TIMEOUT seconds."
			return 124
		else
			return $APP_EXIT_CODE
		fi
	else
		sudo valgrind --leak-check=full --track-origins=yes $ZEPHYR_APP --bt-dev=$HCI_DEVICE
	fi
	return $?
}

retry_count=0
while [ $retry_count -lt $MAX_RETRIES ]; do
	bring_down_hci

	run_valgrind
	exit_code=$?

	if [$exit_code -eq 0]; then
		echo "Zephyr app completed succesfully"
		exit 0
	elif [$exit_code -eq 124]; then
		echo "Zephyr app timed out after $TIMEOUT seconds."
	else
		echo "Zephyr app failed with exit code $exit_code"
	fi

	retry_count=$((retry_count +1))
	echo "Retrying... ($retry_count/$MAX_RETRIES)"
done

echo "Zephyr app failed after $MAX_RETRIES retries. Exiting"
exit 1
