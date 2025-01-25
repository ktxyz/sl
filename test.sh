#!/bin/bash

# Configuration
INTERPRETER="./bin/sl"
VALID_DIR="test/valid"
INVALID_DIR="test/invalid"
CUSTOM_DIR="test/custom"
TMP_OUTPUT="/tmp/sl_test_output"
PASS=0
FAIL=0

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Run valid tests
echo "Running valid tests..."
for test_file in $VALID_DIR/test*.sl; do
    test_name=$(basename "$test_file")
    out_file="$VALID_DIR/${test_name}.out"
    
    if [ ! -f "$out_file" ]; then
        echo -e "${RED}MISSING OUTPUT for $test_name${NC}"
        ((FAIL++))
        continue
    fi

    # Run interpreter and capture output
    $INTERPRETER "$test_file" > "$TMP_OUTPUT" 2>&1
    exit_code=$?

    # Compare results
    diff_result=$(diff -b "$TMP_OUTPUT" "$out_file")
    if [ $exit_code -eq 0 ] && [ -z "$diff_result" ]; then
        echo -e "${GREEN}PASS${NC} $test_name"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC} $test_name"
        echo "Exit code: $exit_code (expected 0)"
        echo "Diff output:"
        echo "$diff_result"
        ((FAIL++))

        $INTERPRETER --ast "$test_file"
    fi
done

# Run custom tests with input
echo -e "\nRunning custom tests..."
for test_file in $CUSTOM_DIR/test*.sl; do
    test_name=$(basename "$test_file")
    input_file="$CUSTOM_DIR/${test_name}.in"
    out_file="$CUSTOM_DIR/${test_name}.out"
    
    if [ ! -f "$out_file" ]; then
        echo -e "${RED}MISSING OUTPUT for $test_name${NC}"
        ((FAIL++))
        continue
    fi

    # Run with input redirection if .in file exists
    if [ -f "$input_file" ]; then
        $INTERPRETER "$test_file" < "$input_file" > "$TMP_OUTPUT" 2>&1
    else
        $INTERPRETER "$test_file" > "$TMP_OUTPUT" 2>&1
    fi
    exit_code=$?

    # Validate results
    diff_result=$(diff -b "$TMP_OUTPUT" "$out_file")
    if [ $exit_code -eq 0 ] && [ -z "$diff_result" ]; then
        echo -e "${GREEN}PASS${NC} $test_name"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC} $test_name"
        echo "Exit code: $exit_code (expected 0)"
        echo "Diff output:"
        echo "$diff_result"
        ((FAIL++))
    fi
done

# Run invalid tests
echo -e "\nRunning invalid tests..."
for test_file in $INVALID_DIR/test*.sl; do
    test_name=$(basename "$test_file")
    
    # Run interpreter and capture exit code
    $INTERPRETER "$test_file" > /dev/null 2>&1
    exit_code=$?

    if [ $exit_code -ne 0 ]; then
        echo -e "${GREEN}PASS${NC} $test_name"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC} $test_name"
        echo "Exit code: $exit_code (expected non-zero)"
        ((FAIL++))
    fi
done

# Summary
echo -e "\nTest summary:"
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"

# Cleanup
rm -f "$TMP_OUTPUT"

# Exit with failure count
exit $FAIL