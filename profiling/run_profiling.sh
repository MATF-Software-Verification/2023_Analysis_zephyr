#!/bin/bash
set -e

ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=${ROOT_DIR}/zephyr

TEST_DIRS=("psa_encrypt_test")
KEY_SIZES=("128" "256")

BUILD_ENABLED=true

if [[ "$1" == "--run-only" ]]; then
	BUILD_ENABLED=false
	echo "Skipping build step. Using existing executables."
fi

# Source the zephyr environment
source ${ZEPHYR_DIR}/zephyr-env.sh

for K_SIZE in "${KEY_SIZES[@]}"; do

	echo "Starting encryption performance test"
	echo " - KEY SIZE ${K_SIZE}"
	# TODO(avra): Add algo flavors

	for DIR in "${TEST_DIRS[@]}"; do
		TEST_NAME=$(basename "$DIR")

		if [ "$BUILD_ENABLED" = true ]; then
			echo "  Building ${TEST_NAME}..."
			LOG_FILE="${DIR}/build_${TEST_NAME}_${K_SIZE}.log"
			west build -p -b native_sim "./${DIR}" > "${LOG_FILE}" 2>&1

			if [ $? -ne 0 ]; then
				echo "  -> Build failed! Logs from build:"
				cat "${LOG_FILE}"
			else
				echo "  -> Build successful. Log saved to ${LOG_FILE}"
			fi
		fi

		echo "  Running performance test..."

		export KEY_SIZE=${K_SIZE}

		sudo -E perf record -g -- ./build/zephyr/zephyr.exe #FIXME "${K_SIZE}"

		RESULT_FILE="results/${TEST_NAME}_${K_SIZE}"

		sudo -E perf report > ${RESULT_FILE}_report.rpt

		#perf script | ${ZEPHYR_DIR}/scripts/profiling/
	done
done

echo "Performance test done"
