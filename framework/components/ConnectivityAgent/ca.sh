#!/bin/bash
ulimit -c unlimited; cd ../bin/; $TEST_EXEC_PREFIX ./IVILinkConnectivityAgent 2>&1| tee ca_$(hostname).log | tee ca_$(hostname)_`date +%Y%m%d-%H%M%S`.log

