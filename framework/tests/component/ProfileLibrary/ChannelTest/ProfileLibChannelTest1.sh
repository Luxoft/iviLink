#!/bin/bash
ulimit -c unlimited; $TEST_EXEC_PREFIX ./ProfileLibChannelTestServer -u /tmp/axis_ca1 -x /tmp/axis_neg1 2>&1| tee ProfileLibChannelTest1.log | tee ProfileLibChannelTest1_`date +%Y%m%d-%H%M%S`.log

