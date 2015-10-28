#!/bin/bash

git status --porcelain | sed -n -e '/??/ d' -e '/^D/ d' -e 's/^ M //' -e '/.*[ch]pp/ p' | xargs astyle --style=gnu --indent=spaces=2

echo -n '# of unimplemented code paths: '
find src -name "*.hpp" -o -name "*.cpp" | xargs grep unimplemented | grep -v '#define' | wc -l
