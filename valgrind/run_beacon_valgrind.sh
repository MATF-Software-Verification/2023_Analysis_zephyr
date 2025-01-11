#!/bin/bash

PROJECT_BASE=$HOME/2023_Analysis_zephyr/

source $PROJECT_BASE/zephyr/zephyr-env.sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <hci_device>"
    echo "Example: $0 hci0"
    exit 1
fi

HCI_DEV=$1

delay=5

west build -p auto -b native_posix $ZEPHYR_BASE/samples/bluetooth/beacon
sudo hciconfig "$hci_device" down
if [ $? -eq 0 ]; then
            echo "HCI device $hci_device is now down."
        fi
        echo "Failed to bring down HCI device $hci_device. Retrying in $delay seconds..."
        sleep "$delay"
sudo hciconfig "$hci_device" down

# sudo ./build/zephyr/zephyr.exe --bt-dev="$HCI_DEV"
valgrind_output_dir=memcheck_beacon

mkdir -p "$valgrind_output_dir"

sudo valgrind --tool=memcheck --leak-check=full --log-file="$valgrind_output_dir/valgrind_$(date +%Y%m%d_%H%M%S).log" \
        $ZEPHYR_BASE/build/zephyr/zephyr.exe --bt-dev="$HCI_DEV" &

sleep 10

sudo killall -9 zephyr.exe