#!/bin/bash
ulimit -c unlimited; cd ../bin/; $TEST_EXEC_PREFIX ./IVILinkNegotiator -u /tmp/axis_ca2 -x /tmp/axis_neg2 -r slave  2>&1| tee neg2.log | tee neg2_`date +%Y%m%d-%H%M%S`.log

