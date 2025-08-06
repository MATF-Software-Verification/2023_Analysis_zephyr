#!/bin/bash
set -e

ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=${ROOT_DIR}/zephyr

TEST_DIRS=("psa_keygen_perf_test" "psa_encrypt_test")
KEY_SIZES=("128" "256")
ALGO_FLAVORS=("CTR" "GCM")

BUILD_ENABLED=true

if [[ "$1" == "--run-only" ]]; then
	BUILD_ENABLED=false
	echo "Skipping build step. Using existing executables."
fi

run_profile_test() {
	local test_dir="$1"
	local key_size="$2"
	local algo_flavor="$3"

	local test_name=$(basename "$test_dir")
	local build_dir="${test_dir}/build"
	local exe_path="${build_dir}/zephyr/zephyr.exe"
	local result_file="${test_dir}/results/${test_name}_${key_size}bit_${algo_flavor}"

	echo "Running ${test_name}"
	echo "  - Key ${key_size}"
	echo "  - Algo ${algo_flavor}"

	if [[ "$BUILD_ENABLED" == true ]]; then
	       echo "  -> Building in ${build_dir}"

	       west build -p -b native_sim -d "${build_dir}" "${test_dir}" #> /dev/null 2>&1
	       if [[ $? -ne 0 ]]; then
		       echo "Build failed!"
		       exit 1
	       fi
	fi

	echo "  -> Running perf..."
	export KEY_SIZE=${key_size}
	export ALGO_FLAVOR=${algo_flavor}
	sudo -E perf record -g -- "${exe_path}"
}

# Source the zephyr environment
source ${ZEPHYR_DIR}/zephyr-env.sh

: <<'END_COMMENT'
for K_SIZE in "${KEY_SIZES[@]}"; do

	echo "Starting encryption performance test"
	echo " - KEY SIZE ${K_SIZE}"
	# TODO(avra): Add algo flavors

	for DIR in "${TEST_DIRS[@]}"; do
		TEST_NAME=$(basename "$DIR")
		BUILD_DIR="${DIR}/build/"
		LOG_FILE="${DIR}/build_${TEST_NAME}_${K_SIZE}.log"

		if [ "$BUILD_ENABLED" = true ]; then
			echo "  Building ${TEST_NAME}..."
			west build -p -b native_sim -d "${BUILD_DIR}" "./${DIR}" > "${LOG_FILE}" 2>&1
			if [ $? -ne 0 ]; then
				echo "  -> Build failed! Logs from build:"
				cat "${LOG_FILE}"
			else
				echo "  -> Build successful. Log saved to ${LOG_FILE}"
			fi
		fi

		echo "  Running performance test..."
		export KEY_SIZE=${K_SIZE}

		sudo -E perf record -g -- "${BUILD_DIR}/zephyr/zephyr.exe"

		RESULT_FILE="results/${TEST_NAME}_${K_SIZE}"

		sudo -E perf report > ${RESULT_FILE}_report.rpt

		#perf script | ${ZEPHYR_DIR}/scripts/profiling/
	done
done
END_COMMENT

for test_name in "${TEST_DIRS[@]}"; do
	for algo in "${ALGO_FLAVORS[@]}"; do
		for size in "${KEY_SIZES[@]}"; do
			run_profile_test "${test_name}" "${algo}" "${size}"
		done
	done
done

echo "Performance test done"
