vnc.txt should be put in /sdcard/demo
run ./adb shell getevent -p to obtain necessary values:
1) determine, which input device is the touchscreen device (usually has the longest description and tags for ABS like 0035 and 0036), its address (/dev/input/event[some number]) is the desired value for "device"
2) "min x" and "max x" are in the ABS section, line starting with 0036 or 0001 (take the corresponding min and max values)
3) "min y" and "max y" are in the ABS section, line starting with 0035 or 0000 (take the corresponding min and max values)
4) "res x" and "res y" are the device's screen resolution measurements

