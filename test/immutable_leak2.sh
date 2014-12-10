#!/bin/bash

$FBU $srcdir/immutable_leak2.fbu 2>&1 | grep -q -F 'assignment leaks mutable pointer'
