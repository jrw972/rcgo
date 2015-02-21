#!/bin/bash

$FBU $srcdir/save_foreign.fbu 2>&1 | grep -q 'assignment leaks pointer to foreign'
