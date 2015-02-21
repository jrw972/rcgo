#!/bin/bash

$FBU $srcdir/immutable_leak.fbu 2>&1 | grep -q -F 'assignment leaks mutable pointers'
