#!/bin/bash

$RC $srcdir/save_foreign.fbu 2>&1 | grep -q 'assignment leaks mutable pointer'
