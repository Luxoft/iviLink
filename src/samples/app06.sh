#!/bin/bash
ulimit -c unlimited; cd ../../src_products/applications/linux/; $TEST_EXEC_PREFIX ./test06_seat_app 2>&1| tee app06_$(hostname).log | tee app06_$(hostname)_`date +%Y%m%d-%H%M%S`.log

