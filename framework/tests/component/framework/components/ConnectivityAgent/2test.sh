#!/bin/bash
ulimit -c unlimited; cd ./TestServer/; $TEST_EXEC_PREFIX ./TestServerApp -u /tmp/axis_ca2 2>&1| tee test2.log | tee test2_`date +%Y%m%d-%H%M%S`.log

