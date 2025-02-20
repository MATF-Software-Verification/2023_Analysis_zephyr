#!/bin/bash

# Start babblesim PHY simulator
cd $BSIM_OUT_PATH/bin/
sudo ./bs_2G4_phy_v1 -s=extended_adv_valgrind_test -D=2

sleep 40

exit 0