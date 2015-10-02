#!/bin/bash

$RC $srcdir/write_immutable.fbu 2>&1 | grep -q 'cannot assign to read-only location'
