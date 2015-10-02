#!/bin/bash

$RC $srcdir/port_leak.fbu 2>&1 | grep -q 'signature leaks pointers'
