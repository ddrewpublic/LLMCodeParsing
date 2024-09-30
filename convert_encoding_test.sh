#!/bin/bash
# convert file of unknown coding to UTF-8
# by looping through *all* encoding types
# then choose one that works!
line=$(printf "=%.0s" {1..50})
for FMT in $(iconv -l); do    
	echo "$line\nFormat $FMT:\n$line"
	iconv -f $FMT -t UTF8 < test_0.txt | head -n20
done > all.txt
