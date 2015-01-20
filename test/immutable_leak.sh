#!/bin/bash

$FBU $srcdir/immutable_leak.fbu 2>&1 | grep -q -F 'cannot convert const $bool to @bool in assignment'
