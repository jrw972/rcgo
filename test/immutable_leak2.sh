#!/bin/bash

$FBU $srcdir/immutable_leak2.fbu 2>&1 | grep -q -F 'values of type const S cannot be assigned because it leaks pointers'
