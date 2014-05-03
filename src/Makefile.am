EXTRA_DIST = scanner.h
BUILT_SOURCES = scanner.h parser.h
AM_LFLAGS = --header=$(addsuffix .h,$(basename $@))
AM_YFLAGS = -d
bin_PROGRAMS = fbu
fbu_SOURCES = main.c scanner.l parser.y \
abstract_value.h abstract_value.c \
codegen.h codegen.c \
compile.h compile.c \
debug.h \
node.h node.c \
reference.h reference.c \
semantics.h semantics.c \
semval.h semval.c \
strtab.h strtab.c \
symtab.h symtab.c \
type.h type.c \
typed_value.h typed_value.c \
untyped_value.h untyped_value.c \
yyparse.h yyparse.c

