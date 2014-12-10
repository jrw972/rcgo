#!/bin/bash

$FBU $srcdir/immutable_leak.fbu 2>&1 | grep -q -F 'cannot convert @$bool to @bool in assignment'
