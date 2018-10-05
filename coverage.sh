#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

src_dir="$1"

# Find .h and .cc files without a corresponding test file.
find "${src_dir}/src" -name "*.cc" -o -name "*.h" |
    grep -v \
	 -e 'test_helpers\.h' \
	 -e 'main\.cc' \
	 -e 'digit_table\.cc' \
	 -e 'letter_table\.cc' \
	 -e 'upper_table\.cc' \
	 -e '.*\_test\.cc' \
	 -e 'unit_tests\.cc' |
    sed -e 's@^.*/@@' -e 's@\.cc$@@' -e 's@\.h$@@' |
    sort -u > expected_tests

./unit_tests --use-colour no -# --list-tags | grep '#' | awk '{ print $2}' | sed -e 's/^\[#//' -e 's/\_test\]$//' | sort > actual_tests_tags

# Run all of the unit tests to generate a coverage report.
find . -name "*.gcda" -delete
./unit_tests --use-colour no
find ./src/.libs -name "*.gcda" | sed -e 's@^.*/librcgo_la-@@' -e 's@\.gcda$@@' | sort > actual_tests_gcda

# Run coverage for each test individually to find indirect testing.
for tst in $(cat actual_tests_tags)
do
    find . -name "*.gcda" -delete
    ./unit_tests --use-colour no -# "[#${tst}_test]"
    if [ -e "${src_dir}/src/${tst}.h" ]
    then
	gcov --relative-only -o "src/unit_tests-${tst}_test.gcda" "src/${tst}.h"
	sed -E -n -e '/NOT_COVERED/ d' -e '/^ *-: *0:Source:.+$$/ p' -e '/^ *#####: *[0-9]+:.*$$/ p' "${tst}.h.gcov"
    fi
    if [ -e "${src_dir}/src/${tst}.cc" ]
    then
	gcov --relative-only "src/.libs/librcgo_la-${tst}.o"
	sed -E -n -e '/NOT_COVERED/ d' -e '/^ *-: *0:Source:.+$$/ p' -e '/^ *#####: *[0-9]+:.*$$/ p' "${tst}.cc.gcov"
    fi
done

printf "\nThe following are untested:\n"
join -v1 expected_tests actual_tests_tags | tee untested

printf "\nThe following were not executed:\n"
join -v1 expected_tests actual_tests_gcda | tee unexecuted

printf "\nThe following TODOs remain:\n"
find "${src_dir}/src" -name "*.cc" -o -name "*.h" | xargs grep -n TODO

printf "\nThe following NOT_COVEREDs remain:\n"
find "${src_dir}/src" -name "*.cc" -o -name "*.h" | xargs grep -n NOT_COVERED

if [ -s untested ] || [ -s unexecuted ]
then
    exit 1
fi
