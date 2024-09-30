

1. Replace consecutive blank lines with single blank line
   - Includes empty lines, or lines with just white space
   ``` sh
   find . -type f -name "*.txt" | xargs -o -I {} sed -r -i '/^\s*$/d' {}
   ```

1. Remove all existing comments into supplementary file
   ``` sh
   sed -e 's://.*::' -e '/\/\*/,/\*\// s/.*//g'
   ```

1. Convert all files to UTF-8 (from windows-1252)
   ``` shell
   for f in $(find . -type f -name "*.txt"); do iconv -f windows-1252 -t utf-8 -o newfile $f && mv -f newfile $f ; done
   ```

1. (Used following script to manually determine likely encoding)
   ``` bash 
   #!/bin/bash
   line=$(printf "=%.0s" {1..50})
   for FMT in $(iconv -l); do    
       echo "$line\nFormat $FMT:\n$line"
       iconv -f $FMT -t UTF8 < test_0.txt | head -n20
   done > all.txt
   ```

1. Replace all smart quotes with standard double quotes
   - NB depends on shell & ```sed``` versions
   ``` sh
   # sed -e -i 's/\(“\|”\)/\x22/g' filename.txt
   find . -type f -name "*.txt" | xargs -o -I {} sed -i -e 's/\(“\|”\)/\x22/g' {}
   ```
