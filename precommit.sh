#!/bin/bash

git status --porcelain | sed -n -e 's/^ M //' -e '/.*[ch]pp/ p' | xargs astyle --style=gnu
