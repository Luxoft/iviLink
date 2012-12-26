copyrightcons - console application for adding copyright comments to many files

usage:

copyrightcons -c license.txt -log logfile [-t] [-i] [-a] -mask mask.txt {dirs/files}

-t: enable test mode modified files will be placed in copies of original files with suffix .test
-i: only report comments to be changed
-a: only report unapproved licenses


---------------------------------------------------
Example of the default case:

copyrightcons -c license.txt -log log.txt -mask mask.txt /path/to/AXIS
---------------------------------------------------
