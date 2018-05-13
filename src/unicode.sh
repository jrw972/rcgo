#!/bin/bash

# Unicode data downloaded from
#   http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt

#rm -f UnicodeData.txt
#wget http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt

(cat license.txt;
cut -d';' -f1,3 UnicodeData.txt | grep -E '(Lu)|(Ll)|(Lm)|(Lt)|(Lo)' | cut -d';' -f1 | awk '
BEGIN {
    blockhead = 0;
    last_x = 0;
    print "#include \"src/utf8.h\"";
    print "namespace rcgo {";
    print "RuneTableEntry letter_table[] = {";
}

{
    x = strtonum ("0x" $1) + 0;
    if (blockhead == 0) {
	blockhead = x;
	last_x = x;
    }
    else if (x - last_x == 1) {
	last_x = x;
    }
    else {
	printf ("{0x%x, 0x%x},\n", blockhead, last_x);
	size++;
	blockhead = x;
	last_x = x;
    }
}

END {
    print "};";
    printf ("size_t letter_table_size = 0x%x;\n", size);
    print "}  // namespace rcgo"
}
') > letter_table.cc

(cat license.txt;
cut -d';' -f1,3 UnicodeData.txt | grep -E '(Nd)' | cut -d';' -f1 | awk '
BEGIN {
    blockhead = 0;
    last_x = 0;
    print "#include \"src/utf8.h\"";
    print "namespace rcgo {";
    print "RuneTableEntry digit_table[] = {";
}

{
    x = strtonum ("0x" $1) + 0;
    if (blockhead == 0) {
	blockhead = x;
	last_x = x;
    }
    else if (x - last_x == 1) {
	last_x = x;
    }
    else {
	printf ("{0x%x, 0x%x},\n", blockhead, last_x);
	size++;
	blockhead = x;
	last_x = x;
    }
}

END {
    print "};";
    printf ("size_t digit_table_size = 0x%x;\n", size);
    print "}  // namespace rcgo"
}') > digit_table.cc

(cat license.txt;
cut -d';' -f1,3 UnicodeData.txt | grep -E '(Lu)' | cut -d';' -f1 | awk '
BEGIN {
    blockhead = 0;
    last_x = 0;
    print "#include \"src/utf8.h\"";
    print "namespace rcgo {";
    print "RuneTableEntry upper_table[] = {";
}

{
    x = strtonum ("0x" $1) + 0;
    if (blockhead == 0) {
	blockhead = x;
	last_x = x;
    }
    else if (x - last_x == 1) {
	last_x = x;
    }
    else {
	printf ("{0x%x, 0x%x},\n", blockhead, last_x);
	size++;
	blockhead = x;
	last_x = x;
    }
}

END {
    print "};";
    printf ("size_t upper_table_size = 0x%x;\n", size);
    print "}  // namespace rcgo"
}') > upper_table.cc
