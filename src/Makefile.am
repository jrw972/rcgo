EXTRA_DIST = scanner.l parser.y
BUILT_SOURCES = scanner.hpp scanner.cpp parser.hpp parser.cpp

scanner.hpp scanner.cpp: scanner.l
	flex --header-file=scanner.hpp --outfile=scanner.cpp scanner.l

parser.hpp parser.cpp: parser.y
	bison --defines=parser.hpp --output=parser.cpp -v -d parser.y

#AM_LFLAGS = --header=$(addsuffix .hpp,$(basename $@))
#AM_YFLAGS = -v -d
bin_PROGRAMS = rc
rc_SOURCES = main.cpp \
action.hpp action.cpp \
ast.hpp ast.cpp \
AstVisitor.hpp \
bind.hpp \
BuiltinFunction.hpp BuiltinFunction.cpp \
Callable.hpp Callable.cpp \
check_types.hpp check_types.cpp \
check_mutability.hpp check_mutability.cpp \
check_constants.hpp check_constants.cpp \
check_control.hpp check_control.cpp \
compute_receiver_access.hpp compute_receiver_access.cpp \
Composition.hpp Composition.cpp \
ComputeReceiverAccess.cpp \
debug.hpp \
enter_symbols.cpp \
enumerate_instances.cpp \
EvaluateStatic.hpp EvaluateStatic.cpp \
executor_base.hpp \
field.hpp \
generate_code.hpp generate_code.cpp \
heap.hpp heap.cpp \
instance_scheduler.hpp instance_scheduler.cpp \
Location.hpp Location.cpp \
MemoryModel.hpp MemoryModel.cpp \
parser.hpp parser.cpp \
partitioned_scheduler.hpp partitioned_scheduler.cpp \
process_definitions.cpp \
process_type_spec.cpp \
process_functions_and_methods.hpp process_functions_and_methods.cpp \
process_types_and_constants.hpp process_types_and_constants.cpp \
reaction.hpp reaction.cpp \
runtime.hpp runtime.cpp \
scanner.hpp scanner.cpp \
semantic.hpp semantic.cpp \
stack.hpp stack.cpp \
Symbol.hpp Symbol.cpp \
SymbolVisitor.hpp \
Template.hpp Template.cpp \
Type.hpp Type.cpp \
TypeCheckExpression.cpp \
typed_value.hpp typed_value.cpp \
types.hpp \
util.hpp util.cpp \
value.hpp value.cpp \
yyparse.hpp yyparse.cpp
rc_LDADD=-lpthread
