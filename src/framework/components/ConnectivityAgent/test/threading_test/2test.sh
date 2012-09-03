#!/bin/bash
ulimit -c unlimited; cd ../../../../../../src_products/framework/components/ConnectivityAgent/test/threading_test/server/; $TEST_EXEC_PREFIX ./ca_threading_test_server -u /tmp/axis_ca2 2>&1| tee test2.log | tee test2_`date +%Y%m%d-%H%M%S`.log

