#!/bin/bash

git status --porcelain | sed -n -e '/??/ d' -e '/^D/ d' -e 's/^ M //' -e '/.*[ch]pp/ p' | xargs astyle --style=gnu
