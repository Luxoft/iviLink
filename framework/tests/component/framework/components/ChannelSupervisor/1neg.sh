#!/bin/bash
ulimit -c unlimited; cd ../../../../framework/components/ChannelSupervisor/NegotiatorProcess/; $TEST_EXEC_PREFIX ./IVILinkNegotiator -u /tmp/axis_ca1 -x /tmp/axis_neg1  2>&1| tee neg1.log | tee neg1_`date +%Y%m%d-%H%M%S`.log

