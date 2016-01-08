#!/bin/bash

find src -name "*.[ch]pp" | xargs astyle --style=gnu --indent=spaces=2
find utest -name "*.[ch]pp" | xargs astyle --style=gnu --indent=spaces=2
find ftest -name "*.[ch]pp" | xargs astyle --style=gnu --indent=spaces=2

echo -n '# of unimplemented code paths: '
find src -name "*.[ch]pp" -o -name "*.[ly]" | xargs grep UNIMPLEMENTED | grep -v '#define' | wc -l

find src -name "*.[ch]pp" -o -name "*.[ly]" | xargs grep -n TODO

find src -name "*.[ch]pp" | xargs grep -n -H error_at_line
