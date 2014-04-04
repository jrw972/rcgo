#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

src_dir="$1"

# Find cpp files without a corresponding test file.
find "${src_dir}/src" -name "*.cpp" | grep -v -e 'main\.cpp' -e 'DigitTable\.cpp' -e 'LetterTable\.cpp' -e 'UpperTable\.cpp' -e 'unit_tests\.cpp' -e '.*\.test\.cpp' | sed -e 's@^.*/@@' -e 's@\.cpp$@@' | sort > expected_tests

./unit_tests --use-colour no -# --list-tags | grep '#' | awk '{ print $2}' | sed -e 's/^\[#//' -e 's/\.test\]$//' | sort > actual_tests_tags

# Run all of the unit tests to generate a coverage report.
find . -name "*.gcda" -delete
./unit_tests --use-colour no
find ./src/.libs -name "*.gcda" | sed -e 's@^.*/librcgo_la-@@' -e 's@\.gcda$@@' | sort > actual_tests_gcda

# Run coverage for each test individually to find indirect testing.
for tst in $(cat actual_tests_tags)
do
    find . -name "*.gcda" -delete
    ./unit_tests --use-colour no -# "[#${tst}.test]"
    gcov --relative-only "src/.libs/librcgo_la-${tst}.o"
    if ! [ -e "${tst}.hpp.gcov" ]
    then
        touch "${tst}.hpp.gcov"
    fi

    sed -E -n -e '/NOT_COVERED/ d' -e '/^ *-: *0:Source:.+$$/ p' -e '/^ *#####: *[0-9]+:.*$$/ p' "${tst}.hpp.gcov" "${tst}.cpp.gcov"
done

printf "\nThe following are untested:\n"
join -v1 expected_tests actual_tests_tags | tee untested

printf "\nThe following were not executed:\n"
join -v1 expected_tests actual_tests_gcda | tee unexecuted

if [ -s untested ] || [ -s unexecuted ]
then
    exit 1
fi
