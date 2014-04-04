#! /bin/sh

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# catch-driver.sh - testsuite driver script for Catch tests

# More information about Catch can be found at
# https://github.com/philsquared/Catch

# This is based on the test-driver distributed with automake

scriptversion=0.1.0

# Make unconditional expansion of undefined variables an error.  This
# helps a lot in preventing typo-related bugs.
set -u

usage_error ()
{
  echo "$0: $*" >&2
  print_usage >&2
  exit 2
}

print_usage ()
{
  cat <<END
Usage:
  catch-driver.sh --test-name=NAME --log-file=PATH --trs-file=PATH
              [--expect-failure={yes|no}] [--color-tests={yes|no}]
              [--enable-hard-errors={yes|no}] [--] PROGRAM
The '--test-name', '--log-file' and '--trs-file' options are mandatory.
END
}

test_name= # Used for reporting.
log_file=  # Where to save the output of the test script.
trs_file=  # Where to save the metadata of the test run.
expect_failure=no
color_tests=no
enable_hard_errors=yes
while test $# -gt 0; do
  case $1 in
  --help) print_usage; exit $?;;
  --version) echo "catch-driver.sh $scriptversion"; exit $?;;
  --test-name) test_name=$2; shift;;
  --log-file) log_file=$2; shift;;
  --trs-file) trs_file=$2; shift;;
  --color-tests) color_tests=$2; shift;;
  --expect-failure) expect_failure=$2; shift;;
  --enable-hard-errors) enable_hard_errors=$2; shift;;
  --) shift; break;;
  -*) usage_error "invalid option: '$1'";;
  esac
  shift
done

if [ -z "$test_name" ]
then
  usage_error "test-name not set"
fi

if [ -z "$log_file" ]
then
  usage_error "log-file not set"
fi

if [ -z "$trs_file" ]
then
  usage_error "trs-file not set"
fi

do_exit='rm -f $log_file $trs_file; (exit $st); exit $st'
trap "st=129; $do_exit" 1
trap "st=130; $do_exit" 2
trap "st=141; $do_exit" 13
trap "st=143; $do_exit" 15

# Test script is run here.
"$@" --reporter console --reporter automake --use-colour $color_tests > $log_file 2>&1
estatus=$?
grep '^:' $log_file > $trs_file
grep -v '^:' $log_file

if test $estatus -ne 0; then
    echo ":test-result: ERROR" > $trs_file
    echo ":global-test-result: ERROR" >> $trs_file
    echo ":recheck: yes" >> $trs_file
    echo ":copy-in-global-log: yes" >> $trs_file
fi

exit 0
