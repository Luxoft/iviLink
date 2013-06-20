#!/bin/bash
ulimit -c unlimited; $TEST_EXEC_PREFIX ./ProfileLibChannelTestClient -u /tmp/axis_ca2 -x /tmp/axis_neg2 2>&1| tee ProfileLibChannelTest2.log | tee ProfileLibChannelTest2_`date +%Y%m%d-%H%M%S`.log

