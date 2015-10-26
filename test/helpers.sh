function expect_success() {
    if rc $srcdir/$3
    then
        echo "ok $1 - $2"
    else
        echo "not ok $1 - $2"
    fi
}

function expect_error() {
    pattern="E$4:"
    if rc $srcdir/$3 2>&1 | grep -q -F "$pattern"
    then
        echo "ok $1 - $2"
    else
        echo "not ok $1 - $2"
    fi
}
