#!/bin/bash

git status --porcelain | sed -n -e '/??/ d' -e '/^D / d' -e '/^ D/ d' -e 's/^ M //' -e 's/^RM.*->//' -e 's/^A //' -e 's/^AM //' -e '/.*[ch]pp/ p' | xargs astyle --style=gnu --indent=spaces=2 --indent-namespaces

echo -n '# of unimplemented code paths: '
find src -name "*.[ch]pp" -o -name "*.[ly]" | xargs grep unimplemented | grep -v '#define' | wc -l

find src -name "*.[ch]pp" -o -name "*.[ly]" | xargs grep -n TODO

find src -name "*.[ch]pp" | xargs grep -o -h -E '\(E[0-9]+\)' | tr -d '()E' | sort -n | uniq -c | awk '$1 > 1 { print "Error " $2 " appears twice in the codebase"; }'

find src -name "*.[ch]pp" | xargs grep -o -h -E '\(E[0-9]+\)'  | tr -d '()E' | sort -n -u | sort > error_list

grep -o -h -E 'E[0-9]+' test/*.sh | tr -d 'E' | sort -n -u | sort > tested_errors

echo "The following errors are not tested:"
join -v 1 error_list tested_errors | sort -n | tr '\n' ' '
echo

echo "Errors tested: $(awk 'END { print NR; }' tested_errors) / $(awk 'END { print NR; }' error_list)"
