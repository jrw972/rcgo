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
BuiltinFunction.hpp BuiltinFunction.cpp \
Callable.hpp Callable.cpp \
construct_symbol_table.cpp \
debug.hpp \
enter_symbols.cpp \
enumerate_instances.cpp \
executor_base.hpp \
field.hpp \
heap.hpp heap.cpp \
instance.hpp \
instance_table.hpp instance_table.cpp \
instance_scheduler.hpp instance_scheduler.cpp \
Location.hpp Location.cpp \
MemoryModel.hpp MemoryModel.cpp \
parameter.hpp \
parser.hpp parser.cpp \
partitioned_scheduler.hpp partitioned_scheduler.cpp \
process_declarations.cpp \
process_definitions.cpp \
process_type_spec.cpp \
scanner.hpp scanner.cpp \
semantic.hpp semantic.cpp \
runtime.hpp runtime.cpp \
stack_frame.hpp stack_frame.cpp \
Symbol.hpp Symbol.cpp \
SymbolVisitor.hpp \
symtab.hpp symtab.cpp \
trigger.hpp \
type.hpp type.cpp \
typed_value.hpp typed_value.cpp \
types.hpp \
util.hpp util.cpp \
value.hpp value.cpp \
yyparse.hpp yyparse.cpp
fbu_LDADD=-lpthread
