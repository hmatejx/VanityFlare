awk '{ print length($0) " " $0; }' N.txt | sort -r -n | cut -d ' ' -f 2- > NAMES.txt
