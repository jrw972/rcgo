#!/bin/bash

$FBU $srcdir/port_leak.fbu 2>&1 | grep -q 'signature leaks pointers'
