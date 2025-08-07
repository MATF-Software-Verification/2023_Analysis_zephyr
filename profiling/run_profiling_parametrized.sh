#!/bin/bash
set -e

FLAMEG_BASE=~/FlameGraph

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
		if [[ ! -v BUILT_DIRS[$test_name] ]]; then
			echo "  -> Building in ${build_dir}"
			west build -p -b native_sim -d "${build_dir}" "${test_dir}" #> /dev/null 2>&1
	       		if [[ $? -ne 0 ]]; then
		       		echo "Build failed!"
		       		exit 1
			else
				echo "Build succeeded."
				BUILT_DIRS[$test_name]=1
			fi
		else
			echo "  Already built, skipping."
		fi
	fi

	echo "  -> Running perf..."
	export KEY_SIZE=${key_size}
	export ALGO_FLAVOR=${algo_flavor}
	if [[ ! -d "${test_dir}/results"  ]]; then
		mkdir -p "${test_dir}/results"
	fi
	echo "  [record]"
	sudo -E perf record -g -o "${result_file}" -- "${exe_path}"
	echo "  [stat]"
	sudo -E perf stat -d -o "${result_file}_stat" -- "${exe_path}"
	echo "  -> Generating flame graph..."
	sudo -E perf script -i "${result_file}" | "${FLAMEG_BASE}/stackcollapse-perf.pl" | "${FLAMEG_BASE}/flamegraph.pl" > "${result_file}_flame.svg"
}

setup_helper() {
	local test_dir="$1"

	if [[ -d ${test_dir}/setup ]]; then
		if [[ "${BUILD_ENABLED}" == true ]]; then
			west build -p always -b native_sim -d ${test_dir}/setup/build ${test_dir}/setup

			mv ${test_dir}/setup/build/zephyr/zephyr.exe ${test_dir}/setup.exe
			rm -rf ${test_dir}/setup/build

			sudo -E ${test_dir}/setup.exe
		fi
	fi
}

ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=${ROOT_DIR}/zephyr

KEY_SIZES=("128" "256")
ALGO_FLAVORS=("CTR" "GCM")

# Source the zephyr environment
source ${ZEPHYR_DIR}/zephyr-env.sh

BUILD_ENABLED=true

if [[ "$1" == "--run-only" ]]; then
	BUILD_ENABLED=false
	echo "Skipping build step. Using existing executables."
fi

for algo in "${ALGO_FLAVORS[@]}"; do
	for size in "${KEY_SIZES[@]}"; do
		run_profile_test "psa_keygen_perf_test" "${size}"  "${algo}"

		run_profile_test "psa_encrypt_test" "${size}" "${algo}"
		run_profile_test "psa_decrypt_test" "${size}" "${algo}"

		setup_helper "psa_key_persistence_test"
		run_profile_test "psa_key_persistence_test" "${size}" "${algo}"
	done
done

echo "Performance test done"
