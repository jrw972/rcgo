#!/bin/bash

$RC $srcdir/same_port.fbu 2>&1 | grep -q 'system is non-deterministic'
