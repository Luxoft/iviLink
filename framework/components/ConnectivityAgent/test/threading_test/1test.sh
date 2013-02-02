#!/bin/bash
ulimit -c unlimited; cd ../../../../../../src_products/framework/components/ConnectivityAgent/test/threading_test/client/; $TEST_EXEC_PREFIX ./ca_threading_test_client -u /tmp/axis_ca1 2>&1| tee test1.log | tee test1_`date +%Y%m%d-%H%M%S`.log

