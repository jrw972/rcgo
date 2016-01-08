#!/bin/bash

function configure() {
    CXXFLAGS="-DCOVERAGE -O0 --coverage" ./configure
}

function cover() {
    find . -name "*.gcda" -delete
    rm -f utest_before.info utest_after.info utest.info
    rm -f ftest_before.info ftest_after.info ftest.info
    rm -rf html

    lcov --capture --initial --base-directory src --directory src --directory src/.libs --output-file utest_before.info --no-external
    pushd utest
    make check
    popd
    lcov --capture --base-directory src --directory src --directory src/.libs --output-file utest_after.info --no-external
    lcov --add-tracefile utest_before.info --add-tracefile utest_after.info --output-file utest.info --no-external
    genhtml -o html/utest utest.info

    lcov --capture --initial --base-directory src --directory src --directory src/.libs --output-file ftest_before.info --no-external
    pushd ftest
    make check
    popd
    lcov --capture --base-directory src --directory src --directory src/.libs --output-file ftest_after.info --no-external
    lcov --add-tracefile ftest_before.info --add-tracefile ftest_after.info --output-file ftest.info --no-external
    genhtml -o html/ftest ftest.info

    echo "file://$PWD/html/utest/index.html"
    echo "file://$PWD/html/ftest/index.html"
}
case $1 in
    configure)
        configure
        ;;
    cover)
        cover
        ;;
    *)
        cover
esac
