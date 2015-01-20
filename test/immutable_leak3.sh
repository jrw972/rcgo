#!/bin/bash

$FBU $srcdir/immutable_leak3.fbu 2>&1 | grep -q -F 'cannot convert $S to @S in assignment'
