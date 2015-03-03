EXTRA_DIST = scanner.l parser.y
BUILT_SOURCES = scanner.hpp scanner.cpp parser.hpp parser.cpp

scanner.hpp scanner.cpp: scanner.l
	flex --header-file=scanner.hpp --outfile=scanner.cpp scanner.l

parser.hpp parser.cpp: parser.y
	bison --defines=parser.hpp --output=parser.cpp -v -d parser.y

#AM_LFLAGS = --header=$(addsuffix .hpp,$(basename $@))
#AM_YFLAGS = -v -d
bin_PROGRAMS = fbu
fbu_SOURCES = main.cpp \
action.hpp \
ast.hpp ast.cpp \
bind.hpp \
construct_symbol_table.cpp \
debug.hpp debug.cpp \
enter_symbols.cpp \
enumerate_instances.cpp \
field.hpp field.cpp \
function.hpp \
method.hpp \
heap.hpp heap.cpp \
instance.hpp instance.cpp \
memory_model.hpp memory_model.cpp \
parameter.hpp \
parser.hpp parser.cpp \
process_declarations.cpp \
process_definitions.cpp \
process_type_spec.cpp \
scanner.hpp scanner.cpp \
semantic.hpp semantic.cpp \
rtstring.hpp rtstring.cpp \
runtime.hpp runtime.cpp \
stack_frame.hpp stack_frame.cpp \
strtab.hpp strtab.cpp \
symbol.hpp symbol.cpp \
symtab.hpp symtab.cpp \
trigger.hpp \
type.hpp type.cpp \
typed_value.hpp typed_value.cpp \
types.hpp \
util.hpp util.cpp \
yyparse.hpp yyparse.cpp
fbu_LDADD=-lpthread
