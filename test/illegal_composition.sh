#!/bin/bash

source $srcdir/helpers.sh

echo 1..4

expect_error 1  "recursive composition" recursive_composition.rc  E72
expect_error 2  "non-determinism (between ports)" same_port.rc E137
expect_error 3  "non-determinism (between reactions)" incompatible_reactions.rc E138
expect_error 4  "non-determinism (write in method)" write_in_method.rc E138
