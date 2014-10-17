EXTRA_DIST = scanner.h
BUILT_SOURCES = scanner.h parser.h
AM_LFLAGS = --header=$(addsuffix .h,$(basename $@))
AM_YFLAGS = -v -d
bin_PROGRAMS = fbu
fbu_SOURCES = main.c scanner.l parser.y \
action.h action.c \
ast.h ast.c \
binding.h binding.c \
debug.h debug.c \
field.h field.c \
instance.h instance.c \
instance_set.h instance_set.c \
parameter.h parameter.c \
semantic.h semantic.c \
semantics.h semantics.c \
strtab.h strtab.c \
symbol.h symbol.c \
symtab.h symtab.c \
trigger.h trigger.c \
type.h type.c \
typed_value.h typed_value.c \
types.h \
util.h util.c \
untyped_value.h untyped_value.c \
yyparse.h yyparse.c
