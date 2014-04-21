EXTRA_DIST = scanner.h
BUILT_SOURCES = scanner.h parser.h
AM_LFLAGS = --header=$(addsuffix .h,$(basename $@))
AM_YFLAGS = -d
bin_PROGRAMS = fbu
fbu_SOURCES = main.c scanner.l parser.y \
codegen.h codegen.c \
compile.h compile.c \
debug.h \
node.h node.c \
semantics.h semantics.c \
semval.h semval.c \
strtab.h strtab.c \
symtab.h symtab.c \
type.h type.c \
yyparse.h yyparse.c

