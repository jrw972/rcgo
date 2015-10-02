#!/bin/bash

$RC $srcdir/immutable_leak3.fbu 2>&1 | grep -q -F 'assignment leaks mutable pointers'
