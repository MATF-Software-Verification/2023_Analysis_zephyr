#!/bin/bash
set -o pipefail

LOG_FILE="${1:-test.log}"
TEMP_DIR="$(mktemp -d)"
declare -A TEST_RESULTS
declare -A TEST_RPAS
declare -A TEST_ERRORS

# Extract test blocks and split by test case
awk '
/START - test_/ { 
    test_name=$3; 
    print > "'"$TEMP_DIR/"'" test_name ".log";
    current=test_name;
    next;
}
/END - test_/ { 
    current=""; 
    next;
}
current { 
    print >> "'"$TEMP_DIR/"'" current ".log";
}' "$LOG_FILE"

# Process each test case
for TEST_LOG in "$TEMP_DIR"/test_*.log; do
    TEST_NAME=$(basename "${TEST_LOG%.log}")
    TEST_RESULTS["$TEST_NAME"]="PASS"
    
    echo -e "\n=== Analyzing $TEST_NAME ==="
    
    # Check for test failure markers
    if grep -q "FAIL - $TEST_NAME" "$LOG_FILE" || 
       grep -q "ASSERTION FAIL" "$TEST_LOG" || 
       grep -q "PROJECT EXECUTION FAILED" "$TEST_LOG"; then
        TEST_RESULTS["$TEST_NAME"]="FAIL"
    fi

    # Extract all RPAs generated
    RPAS=$(grep -oE "Created RPA [0-9A-F]{2}(:[0-9A-F]{2}){5}" "$TEST_LOG" | sort | uniq)
    if [ -n "$RPAS" ]; then
        TEST_RPAS["$TEST_NAME"]="$RPAS"
        echo "Generated RPAs:"
        echo "$RPAS" | sed 's/Created RPA //g'
    else
        echo "[WARN] No RPA generation detected"
    fi

    # Extract all of the RPAs that have been set
    SET_RPAS=$(grep -oE "bt_id_set_adv_random_addr: [0-9A-F]{2}(:[0-9A-F]{2}){5}" "$TEST_LOG" | sort | uniq)
    if [ -n "$SET_RPAS" ]; then
        TEST_SET_RPAS["$TEST_NAME"]="$SET_RPAS"
        echo "Set RPAS:"
        echo "$SET_RPAS" | sed 's/bt_id_set_adv_random_addr: //g'
    else
        echo "[WARN] No RPA setting detected"
    fi

    # Capture critical errors
    ERRORS=$(grep -E "failed|error|assert|zassert" "$TEST_LOG" | grep -v "assert passed")
    if [ -n "$ERRORS" ]; then
        TEST_ERRORS["$TEST_NAME"]="$ERRORS"
        echo -e "\nTest issues:"
        echo "$ERRORS"
    fi

    # Special checks for specific tests
    case "$TEST_NAME" in
        test_rpa_rotation)
            ROTATIONS=$(grep -c "Rotated address" "$TEST_LOG")
            echo "RPA rotations detected: $ROTATIONS"
            ;;
        test_different_irks_yield_different_rpas)
            DIFF_RPAS=$(echo "$RPAS" | wc -l)
            echo "Distinct RPAs generated: $DIFF_RPAS"
            if [ $(grep -coE "bt_rpa: ah: irk [1]{32}" "$TEST_LOG") -eq 0 ]; then
                echo "[ERR] All 1's key not processed"
                TEST_RESULTS["$TEST_NAME"]="${TEST_RESULTS["$TEST_NAME"]} log_parser FAIL: (IRK all 1s missing)"
            fi
            if [ $(grep -coE "bt_rpa: ah: irk [2]{32}" "$TEST_LOG") -eq 0 ]; then
                echo "[ERR] All 2's key not processed"
                TEST_RESULTS["$TEST_NAME"]="${TEST_RESULTS["$TEST_NAME"]} log_parser FAIL: (IRK all 2s missing)"
            fi
            DIFF_RPAS=$(echo "$RPAS" | wc -l)
            if [ $DIFF_RPAS -ne 2 ]; then
                echo "[ERR] Two RPAs haven't been generated!"
            fi
            ;;
        test_rpa_is_stable_before_timeout)
            DIFF_RPAS=$(echo "$RPAS" | wc -l)
            if [ $DIFF_RPAS -gt 1 ]; then
                echo "[ERR] Multiple RPAs generated"
            else
                echo "Single RPA created - PASS"
            fi
            ;;
        test_advertising_address_is_rpa)
            if [ $(grep -coE "bt_rpa: ah: irk deadbeefdeadbeefdeadbeefdeadbeef" "$TEST_LOG") -eq 0 ]; then
                echo "[ERR] Failed to use the proper IRK"
                TEST_RESULTS["$TEST_NAME"]+=" log_parser FAIL: (Known IRK hasn't been used)"
            fi
            ;;
        test_rpa_creation_and_resolution)
            if [ $(grep -coE "bt_rpa_irk_matches: IRK 112233445566778899aabbccddeeff00" "$TEST_LOG") -eq 0 ]; then
                echo "[ERR] IRK match not using the right key"
                TEST_RESULTS["$TEST_NAME"]+=" log_parser FAIL: (Known IRK hasn't been used)"
            fi
            if [ $(grep -coE "bt_rpa_irk_matches: IRK 112233445566778899aabbccddeeffff" "$TEST_LOG") -eq 0 ]; then
                echo "[ERR] IRK match not using the right key"
                TEST_RESULTS["$TEST_NAME"]+=" log_parser FAIL: (Known IRK hasn't been used)"
            fi
            ;;
    esac
done

# Generate final report
echo -e "\n=== Test Suite Summary ==="
printf "%-40s %-8s %s\n" "TEST CASE" "STATUS" "NOTES"
for TEST_NAME in "${!TEST_RESULTS[@]}"; do
    NOTES=""
    
    # Add RPA count for relevant tests
    if [ -n "${TEST_RPAS[$TEST_NAME]}" ]; then
        RPA_COUNT=$(echo "${TEST_RPAS[$TEST_NAME]}" | wc -l)
        NOTES+="RPAs: $RPA_COUNT "
    fi
    
    # Add error snippets
    if [ -n "${TEST_ERRORS[$TEST_NAME]}" ]; then
        FIRST_ERROR=$(echo "${TEST_ERRORS[$TEST_NAME]}" | head -n1 | cut -c1-50)
        NOTES+="Error: $FIRST_ERROR..."
    fi
    
    printf "%-40s %-8s %s\n" "$TEST_NAME" "${TEST_RESULTS[$TEST_NAME]}" "$NOTES"
done

# Cleanup
rm -rf "$TEMP_DIR"

# Exit with failure count
FAIL_COUNT=$(grep -c "FAIL" <<< "${TEST_RESULTS[@]}")
exit $FAIL_COUNT