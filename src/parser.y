%{
#include "scanner.hpp"
#include "yyparse.hpp"
#include "debug.hpp"
#include "node.hpp"
#include "semantic.hpp"

using namespace ast;
%}

%union { ast::Node* node; ast::List* list; ast::Identifier* identifier; Mutability mutability; }

%token <identifier> IDENTIFIER
%token <node> LITERAL
%token <node> STRING_LIT

%type <node> ActionDecl
%type <node> Activate
%type <node> AddExpression
%type <node> AndExpression
%type <node> ArrayDimension
%type <node> Array
%type <node> AssignmentStatement
%type <node> BindDecl
%type <node> BindStatement
%type <node> Block
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
%type <list> IdentifierList
%type <node> If
%type <list> ImportDeclList
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
%type <identifier> PackageClause
%type <identifier> PackageName
%type <node> Parameter
%type <list> ParameterListInternal
%type <node> Pointer
%type <node> PrimaryExpression
%type <node> PullPort
%type <node> PushPortCall
%type <list> PushPortCallList
%type <node> PushPort
%type <node> ReactionDecl
%type <node> Receiver
%type <node> Return
%type <list> ParameterList
%type <list> ReturnParameterList
%type <node> SimpleStatement
%type <node> Slice
%type <node> SourceFile
%type <node> Statement
%type <list> StatementList
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
%token IMPORT
%token INIT
%token INSTANCE
%token INTERFACE
%token MAP
%token PACKAGE_KW
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
%precedence IDENTIFIER

%%

Top: SourceFile { root = $1; }

SourceFile:
PackageClause ';' ImportDeclList TopLevelDeclList { $$ = new SourceFile (@1, $1, $3, $4); }

PackageClause: PACKAGE_KW PackageName { $$ = $2; }

PackageName: IDENTIFIER { $$ = $1; }

ImportDeclList:
  /* Empty */                   { $$ = new ImportDeclList (yyloc); }
| ImportDeclList ImportDecl ';' { UNIMPLEMENTED; }

ImportDecl:
  IMPORT ImportSpec             { UNIMPLEMENTED; }
| IMPORT '(' ImportSpecList ')' { UNIMPLEMENTED; }

ImportSpecList:
  /* Empty */                   { UNIMPLEMENTED; }
| ImportSpecList ImportSpec ';' { UNIMPLEMENTED; }

ImportSpec:
"." ImportPath           { UNIMPLEMENTED; }
| PackageName ImportPath { UNIMPLEMENTED; }
| ImportPath             { UNIMPLEMENTED; }

ImportPath: STRING_LIT { UNIMPLEMENTED; }

TopLevelDeclList:
  /* Empty */                       { $$ = new TopLevelDeclList (yyloc); }
| TopLevelDeclList TopLevelDecl ';' { $$ = $1->append ($2); }

TopLevelDecl:
// No VarDecl as global variables are not allowed.
  ConstDecl    { $$ = $1; }
| TypeDecl     { $$ = $1; }
| FunctionDecl { $$ = $1; }
| MethodDecl   { $$ = $1; }
| InitDecl     { $$ = $1; }
| GetterDecl   { $$ = $1; }
| ActionDecl   { $$ = $1; }
| ReactionDecl { $$ = $1; }
| BindDecl     { $$ = $1; }
| InstanceDecl { $$ = $1; }

ConstDecl:
  CONST IdentifierList Type '=' ExpressionList
  { $$ = new Const (@1, $2, $3, $5); }
| CONST IdentifierList      '=' ExpressionList
  { $$ = new Const (@1, $2, new EmptyType (@1), $4); }

InstanceDecl:
  INSTANCE IDENTIFIER TypeName IDENTIFIER '(' OptionalExpressionList ')'
{ $$ = new Instance (@1, $2, $3, $4, $6); }

TypeDecl: TYPE IDENTIFIER Type { $$ = new ast::Type (@1, $2, $3); }

Mutability:
  /* Empty. */ { $$ = Mutable;   }
| CONST        { $$ = Immutable; }
| FOREIGN      { $$ = Foreign;   }

DereferenceMutability:
  /* Empty. */ { $$ = Mutable;   }
| '$' CONST    { $$ = Immutable; }
| '$' FOREIGN  { $$ = Foreign;   }

Receiver:
  '(' IDENTIFIER Mutability DereferenceMutability '*' IDENTIFIER ')'
{ $$ = new Receiver (@1, $2, $3, $4, true, $6);  }
| '(' IDENTIFIER Mutability DereferenceMutability     IDENTIFIER ')'
{ $$ = new Receiver (@1, $2, $3, $4, false, $5); }

ActionDecl:
  ACTION Receiver IDENTIFIER                '(' Expression ')' Block
{ $$ = new Action (@1, $2, $3, $5, $7); }
| ArrayDimension ACTION Receiver IDENTIFIER '(' Expression ')' Block
{ $$ = new DimensionedAction (@1, $1, $3, $4, $6, $8); }

ReactionDecl:
  REACTION Receiver IDENTIFIER ParameterList Block
{ $$ = new Reaction (@1, $2, $3, $4, $5); }
| ArrayDimension REACTION Receiver IDENTIFIER ParameterList Block
{ $$ = new DimensionedReaction (@2, $1, $3, $4, $5, $6); }

BindDecl:
  BIND Receiver IDENTIFIER Block
{ $$ = new Bind (@1, $2, $3, $4); }

InitDecl:
  INIT Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Initializer (@1, $2, $3, $4, $5, $6); }

GetterDecl:
GETTER Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Getter (@1, $2, $3, $4, $5, $6); }

MethodDecl:
  FUNC Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Method (@1, $2, $3, $4, $5, $6); }

FunctionDecl:
  FUNC IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Function (@1, $2, $3, $4, $5); }

ParameterList:
  '(' ')'
{ $$ = new ParameterList (yyloc); }
| '(' ParameterListInternal optional_semicolon ')'
{ $$ = $2; }

ParameterListInternal:
  Parameter
{ $$ = (new ParameterList (@1))->append ($1); }
| ParameterListInternal ';' Parameter
{ $$ = $1->append ($3); }

Parameter:
  IdentifierList Mutability DereferenceMutability Type
{ $$ = new VariableList (@1, $1, $2, $3, $4); }

ReturnParameterList:
  /* Empty */
{ $$ = new ParameterList (yyloc); }
| DereferenceMutability Type
{ $$ = (new ParameterList (yyloc))->append (new VariableList (@1, (new IdentifierList (@1))->append (new Identifier (@1, "")), Mutable, $1, $2)); }

optional_semicolon: /* Empty. */
| ';'

BindStatement:
  Expression RIGHT_ARROW Expression
{ $$ = new BindPushPort (@1, $1, $3); }
| Expression RIGHT_ARROW Expression DOTDOTDOT Expression
{ $$ = new BindPushPortParameter (@1, $1, $3, $5); }
| Expression LEFT_ARROW Expression
{ $$ = new BindPullPort (@1, $1, $3); }

Block: '{' StatementList '}' { $$ = $2; }

StatementList:
  /* empty */ { $$ = new StatementList (yyloc); }
| StatementList Statement ';' { $$ = $1->append ($2); }

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

EmptyStatement: /* empty */ { $$ = new EmptyStatement (yyloc); }

Activate:
  ACTIVATE OptionalPushPortCallList Block
{ $$ = new Activate (@1, $2, $3); }

Change:
  CHANGE '(' Expression ',' IDENTIFIER ')' Block
{ $$ = new Change (@1, $3, $5, $7); }

ForIota:
  FOR IDENTIFIER DOTDOTDOT Expression Block
{ $$ = new ForIota (@1, $2, $4, $5); }

Return:
  RETURN Expression
{ $$ = new Return (@1, $2); }

IncrementStatement:
  Expression INCREMENT
{ $$ = new IncrementDecrement (@1, $1, IncrementDecrement::Increment); }
| Expression DECREMENT
{ $$ = new IncrementDecrement (@1, $1, IncrementDecrement::Decrement); }

OptionalPushPortCallList:
  /* Empty. */
{ $$ = new ExpressionList (yyloc); }
| PushPortCallList
{ $$ = $1; }

PushPortCallList:
  PushPortCall
{ $$ = (new ExpressionList (@1))->append ($1); }
| PushPortCallList ',' PushPortCall
{ $$ = $1->append ($3); }

PushPortCall:
  IDENTIFIER Indexession '(' OptionalExpressionList ')'
{ $$ = new IndexedPushPortCall (@1, $1, $2, $4); }
| IDENTIFIER '(' OptionalExpressionList ')'
{ $$ = new PushPortCall (@1, $1, $3); }

Indexession: '[' Expression ']' { $$ = $2; }

OptionalExpressionList:
  /* Empty. */   { $$ = new ExpressionList (yyloc); }
| ExpressionList { $$ = $1; }

OptionalTypeOrExpressionList:
  /* Empty. */         { $$ = new ExpressionList (yyloc); }
| TypeOrExpressionList { $$ = $1; }

ExpressionList:
  Expression
{ $$ = (new ExpressionList (@1))->append ($1); }
| ExpressionList ',' Expression
{ $$ = $1->append ($3); }

TypeOrExpressionList:
  Expression
{ $$ = (new ExpressionList (@1))->append ($1); }
| TypeLitExpression
{ $$ = (new ExpressionList (@1))->append (new TypeExpression (@1, $1)); }
| TypeOrExpressionList ',' Expression
{ $$ = $1->append ($3); }
| TypeOrExpressionList ',' TypeLitExpression
{ $$ = $1->append (new TypeExpression (@1, $3)); }

ExpressionStatement:
  Expression
{ $$ = new ExpressionStatement (@1, $1); }

VarDecl:
  VAR IdentifierList Mutability DereferenceMutability Type '=' ExpressionList
{ $$ = new Var (@1, $2, $3, $4, $5, $7); }
| VAR IdentifierList Mutability DereferenceMutability Type
{ $$ = new Var (@1, $2, $3, $4, $5, new ExpressionList (@1)); }
| VAR IdentifierList Mutability DereferenceMutability '=' ExpressionList
{ $$ = new Var (@1, $2, $3, $4, new EmptyType (@1), $6); }
| IdentifierList Mutability DereferenceMutability SHORT_ASSIGN ExpressionList
{ $$ = new Var (@1, $1, $2, $3, new EmptyType (@1), $5); }

AssignmentStatement:
  Expression '=' Expression
{ $$ = new Assign (@1, $1, $3); }
| Expression ADD_ASSIGN Expression
{ $$ = new AddAssign (@1, $1, $3); }

If:
  IF Expression Block
  { $$ = new If (@1, new EmptyStatement (@1), $2, $3, new StatementList (@1)); }
| IF Expression Block ELSE If
{ $$ = new If (@1, new EmptyStatement (@1), $2, $3, $5); }
| IF Expression Block ELSE Block
{ $$ = new If (@1, new EmptyStatement (@1), $2, $3, $5); }
| IF SimpleStatement ';' Expression Block
{ $$ = new If (@1, $2, $4, $5, new StatementList (@1)); }
| IF SimpleStatement ';' Expression Block ELSE If
{ $$ = new If (@1, $2, $4, $5, $7); }
| IF SimpleStatement ';' Expression Block ELSE Block
{ $$ = new If (@1, $2, $4, $5, $7); }

While:
  FOR Expression Block
{ $$ = new While (@1, $2, $3); }

IdentifierList:
  IDENTIFIER
{ $$ = (new IdentifierList (@1))->append ($1); }
| IdentifierList ',' IDENTIFIER
{ $$ = $1->append ($3); }

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
  ArrayDimension ElementType { $$ = new Array (@1, $1, $2); }

ElementType: Type { $$ = $1; }

StructType: STRUCT '{' FieldList '}' { $$ = $3; }

Slice: '[' ']' ElementType { $$ = new Slice (@1, $3); }

Map: MAP '[' KeyType ']' ElementType { $$ = new Map (@1, $3, $5); }

KeyType: Type { $$ = $1; }

ComponentType:
  COMPONENT '{' FieldList '}'
{ $$ = $3; static_cast<FieldList*> ($3)->is_component = true; }

PushPort: PUSH ParameterList { $$ = new PushPort (@1, $2); }

PullPort:
  PULL ParameterList ReturnParameterList
{ $$ = new PullPort (@1, $2, $3); }

Heap: HEAP Type { $$ = new Heap (@1, $2); }

Pointer: '*' Type { $$ = new Pointer (@1, $2); }

TypeLit:
  Pointer       { $$ = $1; }
| TypeLitExpression { $$ = $1; }

TypeName: IDENTIFIER { $$ = new IdentifierType (@1, $1); }

Type:
  TypeName     { $$ = $1; }
| TypeLit      { $$ = $1; }
| '(' Type ')' { $$ = $2; }

ArrayDimension: '[' Expression ']' { $$ = $2; }

FieldList:
  /* empty */
{ $$ = new FieldList (yyloc); }
| FieldList IdentifierList Type ';'
{ $$ = $1->append (new VariableList (@1, $2, Mutable, Mutable, $3)); }

OptionalExpression:
  /* empty */
{ $$ = new EmptyExpression (yyloc); }
| Expression
{ $$ = $1; }

Expression: OrExpression { $$ = $1; }

OrExpression:
  AndExpression
{ $$ = $1; }
| OrExpression LOGIC_OR AndExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::logic_or_temp, $1, $3); }

AndExpression:
  CompareExpression
{ $$ = $1; }
| AndExpression LOGIC_AND CompareExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::logic_and_temp, $1, $3); }

CompareExpression:
  AddExpression
{ $$ = $1; }
| CompareExpression EQUAL AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::equal_temp, $1, $3); }
| CompareExpression NOT_EQUAL AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::not_equal_temp, $1, $3); }
| CompareExpression '<' AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::less_than_temp, $1, $3); }
| CompareExpression LESS_EQUAL AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::less_equal_temp, $1, $3); }
| CompareExpression '>' AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::more_than_temp, $1, $3); }
| CompareExpression MORE_EQUAL AddExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::more_equal_temp, $1, $3); }

AddExpression:
  MultiplyExpression
{ $$ = $1; }
| AddExpression '+' MultiplyExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::add_temp, $1, $3); }
| AddExpression '-' MultiplyExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::subtract_temp, $1, $3); }
| AddExpression '|' MultiplyExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::bit_or_temp, $1, $3); }
| AddExpression '^' MultiplyExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::bit_xor_temp, $1, $3); }

MultiplyExpression:
  UnaryExpression
{ $$ = $1; }
| MultiplyExpression '*' UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::multiply_temp, $1, $3); }
| MultiplyExpression '/' UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::divide_temp, $1, $3); }
| MultiplyExpression '%' UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::modulus_temp, $1, $3); }
| MultiplyExpression LEFT_SHIFT UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::left_shift_temp, $1, $3); }
| MultiplyExpression RIGHT_SHIFT UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::right_shift_temp, $1, $3); }
| MultiplyExpression '&' UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::bit_and_temp, $1, $3); }
| MultiplyExpression AND_NOT UnaryExpression
{ $$ = new ast::BinaryArithmetic (@1, &semantic::bit_and_not_temp, $1, $3); }

UnaryExpression:
  PrimaryExpression   { $$ = $1; }
| '+' UnaryExpression { $$ = new ast::UnaryArithmetic (@1, &semantic::posate_temp, $2); }
| '-' UnaryExpression { $$ = new ast::UnaryArithmetic (@1, &semantic::negate_temp, $2); }
| '!' UnaryExpression { $$ = new ast::UnaryArithmetic (@1, &semantic::logic_not_temp, $2); }
| '^' UnaryExpression { $$ = new ast::UnaryArithmetic (@1, &semantic::complement_temp, $2); }
| '*' UnaryExpression { $$ = new Dereference (@1, $2); }
| '&' UnaryExpression { $$ = new AddressOf (@1, $2); }

PrimaryExpression:
  Literal
{ $$ = $1; }
| TypeLitExpression LiteralValue
{ $$ = new CompositeLiteral (@1, $1, $2); }
| IDENTIFIER LiteralValue
{ $$ = new CompositeLiteral (@1, new IdentifierType (@1, $1), $2); }
| '[' DOTDOTDOT ']' ElementType LiteralValue
{ $$ = new CompositeLiteral (@1, new Array (@1, new EmptyExpression (@1), $4), $5); }
/* | FunctionLit */
| IDENTIFIER
{ $$ = new IdentifierExpression (@1, $1); }
| '(' Expression ')'
{ $$ = $2; }
| PrimaryExpression '.' IDENTIFIER
{ $$ = new Select (@1, $1, $3); }
| PrimaryExpression '[' Expression ']'
{ $$ = new Index (@1, $1, $3); }
| PrimaryExpression '[' OptionalExpression ':' OptionalExpression ']'
{ $$ = new IndexSlice (@1, $1, $3, $5, new EmptyExpression (yyloc)); }
| PrimaryExpression '[' OptionalExpression ':' Expression ':' Expression ']'
{ $$ = new IndexSlice (@1, $1, $3, $5, $7); }
| PrimaryExpression '(' OptionalTypeOrExpressionList ')'
{ $$ = new Call (@1, $1, $3); }
| TypeLitExpression '(' Expression ')'
{ $$ = new Conversion (@1, new TypeExpression (@1, $1), $3); }

Literal:
  LITERAL    { $$ = $1; }
| STRING_LIT { $$ = $1; }

LiteralValue:
  '{' '}'                           { $$ = new ElementList (@1); }
| '{' ElementList OptionalComma '}' { $$ = $2;                          }

ElementList:
  Element                 { $$ = (new ElementList (@1))->append ($1); }
| ElementList ',' Element { $$ = $1->append ($3); }

Element:
  Key ':' Value { $$ = new Element (@1, $1, $3); }
| Value         { $$ = new Element (@1, new EmptyExpression (@1), $1); }

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
