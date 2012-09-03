#!/bin/bash
ulimit -c unlimited; cd ../../../../src_products/framework/components/ConnectivityAgent/linux/; $TEST_EXEC_PREFIX ./IVILinkConnectivityAgent -u /tmp/axis_ca2 2>&1| tee ca2.log | tee ca2_`date +%Y%m%d-%H%M%S`.log

