#!/bin/bash
ulimit -c unlimited; cd ../../../../framework/components/ConnectivityAgent/linux/; $TEST_EXEC_PREFIX ./IVILinkConnectivityAgent -u /tmp/axis_ca1 2>&1| tee ca1.log | tee ca1_`date +%Y%m%d-%H%M%S`.log

