#!/bin/bash

$FBU $srcdir/immutable_leak2.fbu 2>&1 | grep -q -F 'incompatible types (S) = (immutable S)'
