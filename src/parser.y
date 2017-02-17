%define api.pure full
%lex-param {yyscan_t scanner} {const source::SourceFile* source_file}
%parse-param {yyscan_t scanner} {source::SourceFile* source_file}

%{
#include "yyparse.hpp"
#include "debug.hpp"
#include "node.hpp"
#include "source_file.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "polymorphic_function.hpp"

  int yylex (YYSTYPE * yylval_param, YYLTYPE * yylloc, yyscan_t yyscanner, const source::SourceFile* source_file);

 source::Identifier idref (source::Identifier* id)
 {
   source::Identifier retval (*id);
   delete id;
   return retval;
 }

using namespace ast;
%}

%initial-action
{
  yylloc = 1;
};

%union {
  ast::Node* node;
  ast::List* list;
  ast::ParameterList* parameter_list;
  source::Identifier* identifier;
  ast::Receiver* receiver;
  ast::Literal* literal;
  Mutability mutability;
  ast::IdentifierList* identifier_list;
  ast::StatementList* statement_list;
}

%token <identifier> identifier
%token <literal> LITERAL
%token <literal> string_lit

%type <node> ActionDecl
%type <node> Activate
%type <node> AddExpression
%type <node> AndExpression
%type <node> ArrayDimension
%type <node> Array
%type <node> AssignmentStatement
%type <node> BinderDecl
%type <node> BindStatement
%type <statement_list> Block
%type <node> Body
%type <node> Change
%type <node> CompareExpression
%type <node> ComponentType
%type <node> ConstDecl
%type <node> Element
%type <list> ElementList
%type <node> ElementType
%type <node> EmptyStatement
%type <node> Expression
%type <list> ExpressionList
%type <node> ExpressionStatement
%type <list> FieldList
%type <node> ForIota
%type <node> FunctionDecl
%type <node> GetterDecl
%type <node> Heap
%type <identifier_list> IdentifierList
%type <node> If
%type <literal> ImportPath
%type <node> IncrementStatement
%type <node> Indexession
%type <node> InitDecl
%type <node> InstanceDecl
%type <node> Key
%type <node> KeyType
%type <node> Literal
%type <list> LiteralValue
%type <node> Map
%type <node> MethodDecl
%type <node> MultiplyExpression
%type <node> OptionalExpression
%type <list> OptionalExpressionList
%type <list> OptionalPushPortCallList
%type <list> OptionalTypeOrExpressionList
%type <node> OrExpression
%type <identifier> PackageName
%type <node> Parameter
%type <parameter_list> ParameterListInternal
%type <node> Pointer
%type <node> PrimaryExpression
%type <node> PullPort
%type <node> PushPortCall
%type <list> PushPortCallList
%type <node> PushPort
%type <node> ReactionDecl
%type <receiver> Receiver
%type <node> Return
%type <parameter_list> ParameterList
%type <parameter_list> ReturnParameterList
%type <node> SimpleStatement
%type <node> Slice
%type <node> Statement
%type <statement_list> StatementList
%type <node> StructType
%type <node> TopLevelDecl
%type <list> TopLevelDeclList
%type <node> Type
%type <node> TypeDecl
%type <node> TypeLit
%type <node> TypeLitExpression
%type <node> TypeName
%type <list> TypeOrExpressionList
%type <node> UnaryExpression
%type <node> Value
%type <node> VarDecl
%type <node> While

%destructor { /* Free the node ($$). */ } <node>

%type <mutability> Mutability
%type <mutability> DereferenceMutability

// Keywords.
%token ACTION
%token ACTIVATE
%token BIND
%token BREAK
%token CASE
%token CHANGE
%token COMPONENT
%token CONST
%token CONTINUE
%token DEFAULT
%token ELSE
%token FALLTHROUGH
%token FOR
%token FOREIGN
%token FUNC
%token GETTER
%token GOTO
%token HEAP
%token IF
%token import_kw
%token INIT
%token INSTANCE
%token INTERFACE
%token MAP
%token package_kw
%token PULL
%token PUSH
%token RANGE
%token REACTION
%token RETURN
%token STRUCT
%token SWITCH
%token TYPE
%token VAR

// Symbols
%token ADD_ASSIGN
%token AND_ASSIGN
%token AND_NOT
%token AND_NOT_ASSIGN
%token DECREMENT
%token DIVIDE_ASSIGN
%token DOTDOTDOT
%token EQUAL
%token INCREMENT
%token LEFT_ARROW
%token LEFT_SHIFT
%token LEFT_SHIFT_ASSIGN
%token LESS_EQUAL
%token LOGIC_AND
%token LOGIC_OR
%token MODULUS_ASSIGN
%token MORE_EQUAL
%token MULTIPLY_ASSIGN
%token NOT_EQUAL
%token OR_ASSIGN
%token RIGHT_ARROW
%token RIGHT_SHIFT
%token RIGHT_SHIFT_ASSIGN
%token SHORT_ASSIGN
%token SUBTRACT_ASSIGN
%token XOR_ASSIGN

%precedence '{'
%precedence identifier

%%

Top: SourceFile

// https://golang.org/ref/spec#Source_file_organization
SourceFile: PackageClause ';' ImportDeclList TopLevelDeclList { source_file->top_level_decl_list = $4; }

ImportDeclList:
  /* Empty */
| ImportDeclList ImportDecl ';'

TopLevelDeclList:
/* Empty */                         { $$ = new ast::TopLevelDeclList (source_file->location (yyloc)); }
| TopLevelDeclList TopLevelDecl ';' { $$ = $1->append ($2); }

// https://golang.org/ref/spec#Package_clause
PackageClause: package_kw PackageName { source_file->package_name = $2; }
PackageName: identifier { $$ = $1; }

// https://golang.org/ref/spec#Import_declarations
ImportDecl:
  import_kw ImportSpec
| import_kw '(' ImportSpecList ')'

ImportSpecList:
  /* Empty */
| ImportSpecList ImportSpec ';'

ImportSpec:
              ImportPath { source_file->add_import (source::Location (source_file, @1), source::Identifier ("", source::Location (source_file, @1)), $1->eval.value.untyped_string_value.to_string ()); }
|         '.' ImportPath { source_file->add_import (source::Location (source_file, @1), source::Identifier (".", source_file->location (@1)), $2->eval.value.untyped_string_value.to_string ()); }
| PackageName ImportPath { source_file->add_import (source::Location (source_file, @1), idref ($1), $2->eval.value.untyped_string_value.to_string ()); }

ImportPath: string_lit { $$ = $1; }



TopLevelDecl:
// VarDecl not allowed.
  ConstDecl    { $$ = $1; }
| TypeDecl     { $$ = $1; }
| FunctionDecl { $$ = $1; }
| MethodDecl   { $$ = $1; }
| InitDecl     { $$ = $1; }
| GetterDecl   { $$ = $1; }
| ActionDecl   { $$ = $1; }
| ReactionDecl { $$ = $1; }
| BinderDecl   { $$ = $1; }
| InstanceDecl { $$ = $1; }

ConstDecl:
  CONST IdentifierList Type '=' ExpressionList
  { $$ = new ConstDecl (source_file->location (@1), $2, $3, $5); }
| CONST IdentifierList      '=' ExpressionList
  { $$ = new ConstDecl (source_file->location (@1), $2, new EmptyType (source_file->location (@1)), $4); }

InstanceDecl:
  INSTANCE identifier TypeName identifier '(' OptionalExpressionList ')'
{ $$ = new InstanceDecl (source_file->location (@1), idref ($2), $3, idref ($4), $6); }

TypeDecl: TYPE identifier Type { $$ = new ast::TypeDecl (source_file->location (@1), idref ($2), $3); }

Mutability:
  /* Empty. */ { $$ = Mutable;   }
| CONST        { $$ = Immutable; }
| FOREIGN      { $$ = Foreign;   }

DereferenceMutability:
  /* Empty. */ { $$ = Mutable;   }
| '$' CONST    { $$ = Immutable; }
| '$' FOREIGN  { $$ = Foreign;   }

Receiver:
  '(' identifier Mutability DereferenceMutability '*' identifier ')'
  { $$ = new Receiver (source_file->location (@1), idref ($2), $3, $4, true, idref ($6)); }
| '(' identifier Mutability DereferenceMutability     identifier ')'
{ $$ = new Receiver (source_file->location (@1), idref ($2), $3, $4, false, idref ($5)); }

ActionDecl:
  ACTION Receiver identifier                '(' Expression ')' Body
{ $$ = new ActionDecl (source_file->location (@1), new EmptyExpression (source_file->location (@1)), $2, idref ($3), $5, $7); }
| ArrayDimension ACTION Receiver identifier '(' Expression ')' Block
{ $$ = new ActionDecl (source_file->location (@1), $1, $3, idref ($4), $6, $8); }

ReactionDecl:
  REACTION Receiver identifier ParameterList Body
{ $$ = new ReactionDecl (source_file->location (@1), new EmptyExpression (source_file->location (@1)), $2, idref ($3), $4, $5); }
| ArrayDimension REACTION Receiver identifier ParameterList Block
{ $$ = new ReactionDecl (source_file->location (@2), $1, $3, idref ($4), $5, $6); }

BinderDecl:
  BIND Receiver identifier Body
{ $$ = new BinderDecl (source_file->location (@1), $2, idref ($3), $4); }

InitDecl:
  INIT Receiver identifier ParameterList ReturnParameterList Body
{ $$ = new InitializerDecl (source_file->location (@1), $2, idref ($3), $4, $5, $6); }

GetterDecl:
GETTER Receiver identifier ParameterList ReturnParameterList Body
{ $$ = new GetterDecl (source_file->location (@1), $2, idref ($3), $4, $5, $6); }

MethodDecl:
  FUNC Receiver identifier ParameterList ReturnParameterList Body
  { $$ = new MethodDecl (source_file->location (@1), $2, idref ($3), $4, $5, $6); }

FunctionDecl:
  FUNC identifier ParameterList ReturnParameterList Body
{ $$ = new FunctionDecl (source_file->location (@1), idref ($2), $3, $4, $5); }

ParameterList:
  '(' ')'
{ $$ = new ParameterList (source_file->location (yyloc)); }
| '(' ParameterListInternal optional_semicolon ')'
{ $$ = $2; }

ParameterListInternal:
  Parameter
{ $$ = new ParameterList (source_file->location (@1)); $$->append ($1); }
| ParameterListInternal ';' Parameter
{ $$ = $1; $$->append ($3); }

Parameter:
  IdentifierList Mutability DereferenceMutability Type
{ $$ = new VariableList (source_file->location (@1), $1, $2, $3, $4); }

ReturnParameterList:
  /* Empty */
{ $$ = new ParameterList (source_file->location (yyloc)); }
| Type
{ $$ = new ParameterList (source_file->location (yyloc)); $$->append (new VariableList (source_file->location (@1), (new IdentifierList (source_file->location (@1)))->append (source::Identifier ("", source_file->location (@1))), Mutable, Mutable, $1)); }
| '$' CONST Type
{ $$ = new ParameterList (source_file->location (yyloc)); $$->append (new VariableList (source_file->location (@1), (new IdentifierList (source_file->location (@1)))->append (source::Identifier ("", source_file->location (@1))), Mutable, Immutable, $3)); }
| '$' FOREIGN Type
{ $$ = new ParameterList (source_file->location (yyloc)); $$->append (new VariableList (source_file->location (@1), (new IdentifierList (source_file->location (@1)))->append (source::Identifier ("", source_file->location (@1))), Mutable, Foreign, $3)); }

optional_semicolon: /* Empty. */
| ';'

BindStatement:
  Expression RIGHT_ARROW Expression
{ $$ = new BindPushPort (source_file->location (@1), $1, $3); }
| Expression RIGHT_ARROW Expression DOTDOTDOT Expression
{ $$ = new BindPushPortParameter (source_file->location (@1), $1, $3, $5); }
| Expression LEFT_ARROW Expression
{ $$ = new BindPullPort (source_file->location (@1), $1, $3); }

Body: Block { $1->is_body = true; $$ = $1; }

Block: '{' StatementList '}' { $$ = $2; }

StatementList:
  /* empty */ { $$ = new StatementList (source_file->location (yyloc)); }
| StatementList Statement ';' { $1->append ($2); $$ = $1; }

Statement:
  SimpleStatement { $$ = $1; }
| ConstDecl       { $$ = $1; }
| TypeDecl        { $$ = $1; }
| VarDecl         { $$ = $1; }
| Activate        { $$ = $1; }
| Block           { $$ = $1; }
| Return          { $$ = $1; }
| If              { $$ = $1; }
| While           { $$ = $1; }
| Change          { $$ = $1; }
| ForIota         { $$ = $1; }
| BindStatement   { $$ = $1; }

SimpleStatement:
  EmptyStatement      { $$ = $1; }
| ExpressionStatement { $$ = $1; }
| IncrementStatement  { $$ = $1; }
| AssignmentStatement { $$ = $1; }

EmptyStatement: /* empty */ { $$ = new EmptyStatement (source_file->location (yyloc)); }

Activate:
  ACTIVATE OptionalPushPortCallList Block
{ $$ = new Activate (source_file->location (@1), $2, $3); }

Change:
  CHANGE '(' Expression ',' identifier ')' Block
{ $$ = new Change (source_file->location (@1), $3, idref ($5), $7); }

ForIota:
  FOR identifier DOTDOTDOT Expression Block
{ $$ = new ForIota (source_file->location (@1), idref ($2), $4, $5); }

Return:
  RETURN Expression
{ $$ = new Return (source_file->location (@1), $2); }

IncrementStatement:
  Expression INCREMENT
{ $$ = new IncrementDecrement (source_file->location (@1), $1, IncrementDecrement::Increment); }
| Expression DECREMENT
{ $$ = new IncrementDecrement (source_file->location (@1), $1, IncrementDecrement::Decrement); }

OptionalPushPortCallList:
  /* Empty. */
{ $$ = new ExpressionList (source_file->location (yyloc)); }
| PushPortCallList
{ $$ = $1; }

PushPortCallList:
  PushPortCall
{ $$ = (new ExpressionList (source_file->location (@1)))->append ($1); }
| PushPortCallList ',' PushPortCall
{ $$ = $1->append ($3); }

PushPortCall:
  identifier Indexession '(' OptionalExpressionList ')'
{ $$ = new IndexedPushPortCall (source_file->location (@1), idref ($1), $2, $4); }
| identifier '(' OptionalExpressionList ')'
{ $$ = new PushPortCall (source_file->location (@1), idref ($1), $3); }

Indexession: '[' Expression ']' { $$ = $2; }

OptionalExpressionList:
  /* Empty. */   { $$ = new ExpressionList (source_file->location (yyloc)); }
| ExpressionList { $$ = $1; }

OptionalTypeOrExpressionList:
  /* Empty. */         { $$ = new ExpressionList (source_file->location (yyloc)); }
| TypeOrExpressionList { $$ = $1; }

ExpressionList:
  Expression
{ $$ = (new ExpressionList (source_file->location (@1)))->append ($1); }
| ExpressionList ',' Expression
{ $$ = $1->append ($3); }

TypeOrExpressionList:
  Expression
{ $$ = (new ExpressionList (source_file->location (@1)))->append ($1); }
| TypeLitExpression
{ $$ = (new ExpressionList (source_file->location (@1)))->append (new TypeExpression (source_file->location (@1), $1)); }
| TypeOrExpressionList ',' Expression
{ $$ = $1->append ($3); }
| TypeOrExpressionList ',' TypeLitExpression
{ $$ = $1->append (new TypeExpression (source_file->location (@1), $3)); }

ExpressionStatement:
  Expression
{ $$ = new ExpressionStatement (source_file->location (@1), $1); }

VarDecl:
  VAR IdentifierList Mutability DereferenceMutability Type '=' ExpressionList
{ $$ = new VarDecl (source_file->location (@1), $2, $3, $4, $5, $7); }
| VAR IdentifierList Mutability DereferenceMutability Type
{ $$ = new VarDecl (source_file->location (@1), $2, $3, $4, $5, new ExpressionList (source_file->location (@1))); }
| VAR IdentifierList Mutability DereferenceMutability '=' ExpressionList
{ $$ = new VarDecl (source_file->location (@1), $2, $3, $4, new EmptyType (source_file->location (@1)), $6); }
| IdentifierList Mutability DereferenceMutability SHORT_ASSIGN ExpressionList
{ $$ = new VarDecl (source_file->location (@1), $1, $2, $3, new EmptyType (source_file->location (@1)), $5); }

AssignmentStatement:
  Expression '=' Expression
{ $$ = new Assign (source_file->location (@1), $1, $3); }
| Expression ADD_ASSIGN Expression
{ $$ = new AddAssign (source_file->location (@1), $1, $3); }

If:
  IF Expression Block
  { $$ = new If (source_file->location (@1), new EmptyStatement (source_file->location (@1)), $2, $3, new StatementList (source_file->location (@1))); }
| IF Expression Block ELSE If
{ $$ = new If (source_file->location (@1), new EmptyStatement (source_file->location (@1)), $2, $3, $5); }
| IF Expression Block ELSE Block
{ $$ = new If (source_file->location (@1), new EmptyStatement (source_file->location (@1)), $2, $3, $5); }
| IF SimpleStatement ';' Expression Block
{ $$ = new If (source_file->location (@1), $2, $4, $5, new StatementList (source_file->location (@1))); }
| IF SimpleStatement ';' Expression Block ELSE If
{ $$ = new If (source_file->location (@1), $2, $4, $5, $7); }
| IF SimpleStatement ';' Expression Block ELSE Block
{ $$ = new If (source_file->location (@1), $2, $4, $5, $7); }

While:
  FOR Expression Block
{ $$ = new While (source_file->location (@1), $2, $3); }

IdentifierList:
  identifier
{ $$ = (new IdentifierList (source_file->location (@1)))->append (idref ($1)); }
| IdentifierList ',' identifier
{ $$ = $1->append (idref ($3)); }

// Type literals that can appear in expressions.
TypeLitExpression:
  Array     { $$ = $1; }
| StructType    { $$ = $1; }
| Slice     { $$ = $1; }
| Map       { $$ = $1; }
| ComponentType { $$ = $1; }
| PushPort  { $$ = $1; }
| PullPort  { $$ = $1; }
| Heap      { $$ = $1; }

Array:
  ArrayDimension ElementType { $$ = new Array (source_file->location (@1), $1, $2); }

ElementType: Type { $$ = $1; }

StructType: STRUCT '{' FieldList '}' { $$ = $3; }

Slice: '[' ']' ElementType { $$ = new Slice (source_file->location (@1), $3); }

Map: MAP '[' KeyType ']' ElementType { $$ = new Map (source_file->location (@1), $3, $5); }

KeyType: Type { $$ = $1; }

ComponentType:
  COMPONENT '{' FieldList '}'
{ $$ = $3; static_cast<FieldList*> ($3)->is_component = true; }

PushPort: PUSH ParameterList { $$ = new PushPort (source_file->location (@1), $2); }

PullPort:
  PULL ParameterList ReturnParameterList
{ $$ = new PullPort (source_file->location (@1), $2, $3); }

Heap: HEAP Type { $$ = new Heap (source_file->location (@1), $2); }

Pointer: '*' Type { $$ = new Pointer (source_file->location (@1), $2); }

TypeLit:
  Pointer       { $$ = $1; }
| TypeLitExpression { $$ = $1; }

TypeName: identifier { $$ = new IdentifierType (source_file->location (@1), idref ($1)); }

Type:
  TypeName     { $$ = $1; }
| TypeLit      { $$ = $1; }
| '(' Type ')' { $$ = $2; }

ArrayDimension: '[' Expression ']' { $$ = $2; }

FieldList:
  /* empty */
{ $$ = new FieldList (source_file->location (yyloc)); }
| FieldList IdentifierList Type ';'
{ $$ = $1->append (new VariableList (source_file->location (@1), $2, Mutable, Mutable, $3)); }

OptionalExpression:
  /* empty */
{ $$ = new EmptyExpression (source_file->location (yyloc)); }
| Expression
{ $$ = $1; }

Expression: OrExpression { $$ = $1; }

OrExpression:
  AndExpression
{ $$ = $1; }
| OrExpression LOGIC_OR AndExpression
{ $$ = make_binary (source_file->location (@1), &decl::logic_or, $1, $3); }

AndExpression:
  CompareExpression
{ $$ = $1; }
| AndExpression LOGIC_AND CompareExpression
{ $$ = make_binary (source_file->location (@1), &decl::logic_and, $1, $3); }

CompareExpression:
  AddExpression
{ $$ = $1; }
| CompareExpression EQUAL AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::equal, $1, $3); }
| CompareExpression NOT_EQUAL AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::not_equal, $1, $3); }
| CompareExpression '<' AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::less_than, $1, $3); }
| CompareExpression LESS_EQUAL AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::less_equal, $1, $3); }
| CompareExpression '>' AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::more_than, $1, $3); }
| CompareExpression MORE_EQUAL AddExpression
{ $$ = make_binary (source_file->location (@1), &decl::more_equal, $1, $3); }

AddExpression:
  MultiplyExpression
{ $$ = $1; }
| AddExpression '+' MultiplyExpression
{ $$ = make_binary (source_file->location (@1), &decl::add, $1, $3); }
| AddExpression '-' MultiplyExpression
{ $$ = make_binary (source_file->location (@1), &decl::subtract, $1, $3); }
| AddExpression '|' MultiplyExpression
{ $$ = make_binary (source_file->location (@1), &decl::bit_or, $1, $3); }
| AddExpression '^' MultiplyExpression
{ $$ = make_binary (source_file->location (@1), &decl::bit_xor, $1, $3); }

MultiplyExpression:
  UnaryExpression
{ $$ = $1; }
| MultiplyExpression '*' UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::multiply, $1, $3); }
| MultiplyExpression '/' UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::divide, $1, $3); }
| MultiplyExpression '%' UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::modulus, $1, $3); }
| MultiplyExpression LEFT_SHIFT UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::left_shift, $1, $3); }
| MultiplyExpression RIGHT_SHIFT UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::right_shift, $1, $3); }
| MultiplyExpression '&' UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::bit_and, $1, $3); }
| MultiplyExpression AND_NOT UnaryExpression
{ $$ = make_binary (source_file->location (@1), &decl::bit_and_not, $1, $3); }

UnaryExpression:
  PrimaryExpression   { $$ = $1; }
| '+' UnaryExpression { $$ = make_unary (source_file->location (@1), &decl::posate, $2); }
| '-' UnaryExpression { $$ = make_unary (source_file->location (@1), &decl::negate, $2); }
| '!' UnaryExpression { $$ = make_unary (source_file->location (@1), &decl::logic_not, $2); }
| '^' UnaryExpression { $$ = make_unary (source_file->location (@1), &decl::complement, $2); }
| '*' UnaryExpression { $$ = new Dereference (source_file->location (@1), $2); }
| '&' UnaryExpression { $$ = new AddressOf (source_file->location (@1), $2); }

PrimaryExpression:
  Literal
{ $$ = $1; }
| TypeLitExpression LiteralValue
{ $$ = new CompositeLiteral (source_file->location (@1), $1, $2); }
| identifier LiteralValue
{ $$ = new CompositeLiteral (source_file->location (@1), new IdentifierType (source_file->location (@1), idref ($1)), $2); }
| '[' DOTDOTDOT ']' ElementType LiteralValue
{ $$ = new CompositeLiteral (source_file->location (@1), new Array (source_file->location (@1), new EmptyExpression (source_file->location (@1)), $4), $5); }
/* | FunctionLit */
| identifier
{ $$ = new IdentifierExpression (source_file->location (@1), idref ($1)); }
| '(' Expression ')'
{ $$ = $2; }
| PrimaryExpression '.' identifier
{ $$ = new Select (source_file->location (@1), $1, idref ($3)); }
| PrimaryExpression '[' Expression ']'
{ $$ = new Index (source_file->location (@1), $1, $3); }
| PrimaryExpression '[' OptionalExpression ':' OptionalExpression ']'
{ $$ = new IndexSlice (source_file->location (@1), $1, $3, $5, new EmptyExpression (source_file->location (yyloc))); }
| PrimaryExpression '[' OptionalExpression ':' Expression ':' Expression ']'
{ $$ = new IndexSlice (source_file->location (@1), $1, $3, $5, $7); }
| PrimaryExpression '(' OptionalTypeOrExpressionList ')'
{ $$ = new Call (source_file->location (@1), $1, $3); }
| TypeLitExpression '(' Expression ')'
{ $$ = new Conversion (source_file->location (@1), new TypeExpression (source_file->location (@1), $1), $3); }

Literal:
  LITERAL    { $$ = $1; }
| string_lit { $$ = $1; }

LiteralValue:
  '{' '}'                           { $$ = new ElementList (source_file->location (@1)); }
| '{' ElementList OptionalComma '}' { $$ = $2;                          }

ElementList:
  Element                 { $$ = (new ElementList (source_file->location (@1)))->append ($1); }
| ElementList ',' Element { $$ = $1->append ($3); }

Element:
  Key ':' Value { $$ = new Element (source_file->location (@1), $1, $3); }
| Value         { $$ = new Element (source_file->location (@1), new EmptyExpression (source_file->location (@1)), $1); }

Key:
  Expression   { $$ = $1; }
| LiteralValue { $$ = $1; }

Value:
  Expression   { $$ = $1; }
| LiteralValue { $$ = $1; }

OptionalComma:
  /* empty */
| ','

%%
