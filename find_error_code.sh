#!/bin/bash

grep -o -h -E 'E[0-9]+:' src/*.hpp src/*.cpp | sed -e 's/^E//' -e 's/:$//' | sort -n | awk '{ if (NR < $0) { print NR; exit; } } END { print NR + 1; }'
