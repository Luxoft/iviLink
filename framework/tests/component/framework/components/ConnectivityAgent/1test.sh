#!/bin/bash
ulimit -c unlimited; cd ./TestClient/; $TEST_EXEC_PREFIX ./TestClientApp -u /tmp/axis_ca1 2>&1| tee test1.log | tee test1_`date +%Y%m%d-%H%M%S`.log

