#!/bin/bash

# Define variables for directories and paths
ROOT_DIR=~/2023_Analysis_zephyr
ZEPHYR_DIR=~/2023_Analysis_zephyr/zephyr
TEST_DIR=~/2023_Analysis_zephyr/testing/unit/bt_encrypt_decrypt
PATCH_DIR=~/2023_Analysis_zephyr/patches
PATCH_FILE=${PATCH_DIR}/bt_crypto.patch

# Ensure the patch directory exists
echo "Creating the patch dir..."
mkdir -p $PATCH_DIR

# Cleanup twister-out folders
echo "Cleaning up twister-out..."
cd $ROOT_DIR/
rm -rf twister-out*

# Change directory to the root directory of the Zephyr project
echo "Changing dir to zephyr test dir..."
cd $ZEPHYR_DIR/tests/bluetooth/host/crypto/

# Create a patch for the test
#echo "git diff"
#git diff > $PATCH_FILE

# Apply the patch
echo "git apply"
git apply $PATCH_FILE

# Change back to the 2023_Analysis_zephyr directory
cd ~/2023_Analysis_zephyr

# Source Zephyr environment setup script
source zephyr/zephyr-env.sh

# Run twister unit tests with coverage report and copy the resulting folder
twister --coverage -p unit_testing -T zephyr/tests/bluetooth/host/crypto/ -v
cp -r twister-out $TEST_DIR

# Revert the patch after tests are complete
cd $ZEPHYR_DIR/tests/bluetooth/host/crypto/
git apply -R $PATCH_FILE

# Open Firefox with coverage report
cd $TEST_DIR
firefox ./twister-out/coverage/index.html &

