#!/bin/bash

source $srcdir/helpers.sh

# Test that signatures are foreign safe.
# Currently, five signatures must be foreign safe:
#   initializers
#   push ports
#   reactions
#   pull ports
#   getters

echo 1..7

expect_error 1 "initializers must be foreign safe (parameters)" foreign_safe_initializer.rc E9
#expect_success 2 "initializers must be foreign safe (return)"     foreign_safe_initializer_return.rc
expect_error 2 "push ports must be foreign safe"                foreign_safe_push_port.rc E9
expect_error 3 "reactions must be foreign safe"                 foreign_safe_reaction.rc E9
expect_error 4 "pull ports must be foreign safe (parameters)"   foreign_safe_pull_port.rc E9
expect_error 5 "pull ports must be foreign safe (return)"       foreign_safe_pull_port_return.rc E9
expect_error 6 "getters must be foreign safe (parameters)"      foreign_safe_getter.rc E9
expect_error 7 "getters must be foreign safe (return)"          foreign_safe_getter_return.rc E9
