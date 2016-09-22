func function_name(        name) {
    name = ""
    for (idx = 0; idx < count; idx += 1) {
        name = name type[idx] "_" id[idx] "_"
    }
    return name
}

func unit_name(         name) {
    name = ""
    for (idx = 0; idx < count; idx += 1) {
        if (type[idx] == "scope" || type[idx] == "unit") {
            if (name != "") {
                name = name "::"
            }
            name = name id[idx]
        }
    }
    return name
}

BEGIN {
    count = 0
}

/^[ ]*\/\// {
    next;
}

/scope[ ]*\([ ]*[a-zA-z][a-zA-Z0-9_]*[ ]*\)/ {
    sub(/.*scope[ ]*\(/, "", $0)
    sub(/[^a-zA-Z0-9_].*/, "", $0)
    id[count] = $0
    type[count] = "scope"
    code[count] = ""
    count += 1
    next
}

/unit[ ]*\([ ]*[a-zA-z][a-zA-Z0-9_]*[ ]*\)/ {
    sub(/.*unit[ ]*\(/, "", $0)
    sub(/[^a-zA-Z0-9_].*/, "", $0)
    id[count] = $0
    type[count] = "unit"
    code[count] = ""
    count += 1
    next
}

/scenario[ ]*\([ ]*[a-zA-z][a-zA-Z0-9_]*[ ]*\)/ {
    sub(/.*scenario[ ]*\(/, "", $0)
    sub(/[^a-zA-Z0-9_].*/, "", $0)
    id[count] = $0
    type[count] = "scenario"
    code[count] = ""
    count += 1
    next
}

/off[ ]*\([ ]*\)/ {
    if (type[count-1] == "unit") {
        name = function_name()
        print "void " name " () {"
        for (idx = 0; idx < count; idx += 1) {
            print code[idx]
        }
        print "}"
        print "RegisterTestUnit register_test_unit_" name " (" name ", \"" unit_name() "\");"

        for (idx = 0; idx < count; idx += 1) {
            if (type[idx] == "scenario") {
                print "RegisterTestScenario register_test_scenario_" name " (" name, ", \"" id[idx] "\");"
            }
        }
    }
    count -= 1
    next
}

{
    if (count > 0) {
        code[count - 1] = code[count - 1] "#line " NR " \"" FILENAME "\"\n"
        code[count - 1] = code[count - 1] $0 "\n"
    } else {
        print "#line " NR " \"" FILENAME "\""
        print
    }
}
