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

%type <node> Action
%type <node> ActivateStatement
%type <node> AddExpression
%type <node> AndExpression
%type <node> ArrayDimension
%type <node> ArrayType
%type <node> AssignmentStatement
%type <node> Bind
%type <node> BindStatement
%type <node> Block
%type <node> ChangeStatement
%type <node> CompareExpression
%type <node> ComponentType
%type <node> Const
%type <node> Definition
%type <list> DefinitionList
%type <node> Element
%type <list> ElementList
%type <node> ElementType
%type <node> EmptyStatement
%type <node> Expression
%type <list> ExpressionList
%type <node> ExpressionStatement
%type <list> FieldList
%type <node> ForIotaStatement
%type <node> Func
%type <node> Getter
%type <node> HeapType
%type <list> IdentifierList
%type <node> IfStatement
%type <node> IncrementStatement
%type <node> IndexExpression
%type <node> Init
%type <node> Instance
%type <node> Key
%type <node> KeyType
%type <list> LiteralValue
%type <node> MapType
%type <node> Method
%type <node> MultiplyExpression
%type <node> OptionalExpression
%type <list> OptionalExpressionList
%type <list> OptionalPushPortCallList
%type <list> OptionalTypeOrExpressionList
%type <node> OrExpression
%type <node> Parameter
%type <list> ParameterListInternal
%type <node> PointerType
%type <node> PrimaryExpression
%type <node> PullPortType
%type <node> PushPortCall
%type <list> PushPortCallList
%type <node> PushPortType
%type <node> Reaction
%type <node> Receiver
%type <node> ReturnStatement
%type <list> ParameterList
%type <list> ReturnParameterList
%type <node> SimpleStatement
%type <node> SliceType
%type <node> Statement
%type <list> StatementList
%type <node> StructType
%type <node> Type
%type <node> TypeDecl
%type <node> TypeLit
%type <node> TypeLitExpression
%type <node> TypeName
%type <list> TypeOrExpressionList
%type <node> UnaryExpression
%type <node> Value
%type <node> VarStatement
%type <node> WhileStatement

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
%token INIT
%token INSTANCE
%token INTERFACE
%token MAP
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

Top: DefinitionList { root = $1; }

DefinitionList:
  /* empty */               { $$ = new SourceFile (); }
| DefinitionList Definition { $$ = $1->append ($2);   }

Definition:
  TypeDecl { $$ = $1; }
| Init     { $$ = $1; }
| Getter   { $$ = $1; }
| Action   { $$ = $1; }
| Reaction { $$ = $1; }
| Bind     { $$ = $1; }
| Instance { $$ = $1; }
| Method   { $$ = $1; }
| Func     { $$ = $1; }
| Const    { $$ = $1; }

Const:
  CONST IdentifierList Type '=' ExpressionList ';'
  { $$ = new Const (@1, $2, $3, $5); }
| CONST IdentifierList      '=' ExpressionList ';'
  { $$ = new Const (@1, $2, new EmptyTypeSpec (@1), $4); }

Instance:
  INSTANCE IDENTIFIER TypeName IDENTIFIER '(' OptionalExpressionList ')' ';'
{ $$ = new Instance (@1, $2, $3, $4, $6); }

TypeDecl: TYPE IDENTIFIER Type ';' { $$ = new ast::Type (@1, $2, $3); }

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

Action:
  ACTION Receiver IDENTIFIER                '(' Expression ')' Block
{ $$ = new Action (@1, $2, $3, $5, $7); }
| ArrayDimension ACTION Receiver IDENTIFIER '(' Expression ')' Block
{ $$ = new DimensionedAction (@1, $1, $3, $4, $6, $8); }

Reaction:
  REACTION Receiver IDENTIFIER ParameterList Block
{ $$ = new Reaction (@1, $2, $3, $4, $5); }
| ArrayDimension REACTION Receiver IDENTIFIER ParameterList Block
{ $$ = new DimensionedReaction (@2, $1, $3, $4, $5, $6); }

Bind:
  BIND Receiver IDENTIFIER Block
{ $$ = new Bind (@1, $2, $3, $4); }

Init:
  INIT Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Initializer (@1, $2, $3, $4, $5, $6); }

Getter:
GETTER Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Getter (@1, $2, $3, $4, $5, $6); }

Method:
  FUNC Receiver IDENTIFIER ParameterList ReturnParameterList Block
{ $$ = new Method (@1, $2, $3, $4, $5, $6); }

Func:
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
{ $$ = new IdentifierListTypeSpec (@1, $1, $2, $3, $4); }

ReturnParameterList:
  /* Empty */
{ $$ = new ParameterList (yyloc); }
| DereferenceMutability Type
{ $$ = (new ParameterList (yyloc))->append (new IdentifierListTypeSpec (@1, (new IdentifierList (@1))->append (new Identifier (@1, "")), Mutable, $1, $2)); }

optional_semicolon: /* Empty. */
| ';'

BindStatement:
  Expression RIGHT_ARROW Expression ';'
{ $$ = new BindPushPortStatement (@1, $1, $3); }
| Expression RIGHT_ARROW Expression DOTDOTDOT Expression';'
{ $$ = new BindPushPortParamStatement (@1, $1, $3, $5); }
| Expression LEFT_ARROW Expression ';'
{ $$ = new BindPullPortStatement (@1, $1, $3); }

Block: '{' StatementList '}' { $$ = $2; }

StatementList:
  /* empty */ { $$ = new ListStatement (yyloc); }
| StatementList Statement { $$ = $1->append ($2); }

Statement:
  SimpleStatement   { $$ = $1; }
| VarStatement      { $$ = $1; }
| ActivateStatement { $$ = $1; }
| Block             { $$ = $1; }
| ReturnStatement   { $$ = $1; }
| IfStatement       { $$ = $1; }
| WhileStatement    { $$ = $1; }
| ChangeStatement   { $$ = $1; }
| ForIotaStatement  { $$ = $1; }
| BindStatement     { $$ = $1; }
| Const             { $$ = $1; }

SimpleStatement:
  EmptyStatement      { $$ = $1; }
| ExpressionStatement { $$ = $1; }
| IncrementStatement  { $$ = $1; }
| AssignmentStatement { $$ = $1; }

EmptyStatement: /* empty */ ';' { $$ = new EmptyStatement (yyloc); }

ActivateStatement:
  ACTIVATE OptionalPushPortCallList Block
{ $$ = new ActivateStatement (@1, $2, $3); }

ChangeStatement:
  CHANGE '(' Expression ',' IDENTIFIER ')' Block
{ $$ = new ChangeStatement (@1, $3, $5, $7); }

ForIotaStatement:
  FOR IDENTIFIER DOTDOTDOT Expression Block
{ $$ = new ForIotaStatement (@1, $2, $4, $5); }

ReturnStatement:
  RETURN Expression ';'
{ $$ = new ReturnStatement (@1, $2); }

IncrementStatement:
  Expression INCREMENT ';'
{ $$ = new IncrementDecrementStatement (@1, $1, IncrementDecrementStatement::Increment); }
| Expression DECREMENT ';'
{ $$ = new IncrementDecrementStatement (@1, $1, IncrementDecrementStatement::Decrement); }

OptionalPushPortCallList:
  /* Empty. */
{ $$ = new ListExpr (yyloc); }
| PushPortCallList
{ $$ = $1; }

PushPortCallList:
  PushPortCall
{ $$ = (new ListExpr (@1))->append ($1); }
| PushPortCallList ',' PushPortCall
{ $$ = $1->append ($3); }

PushPortCall:
  IDENTIFIER IndexExpression '(' OptionalExpressionList ')'
{ $$ = new IndexedPushPortCallExpr (@1, $1, $2, $4); }
| IDENTIFIER '(' OptionalExpressionList ')'
{ $$ = new PushPortCallExpr (@1, $1, $3); }

IndexExpression: '[' Expression ']' { $$ = $2; }

OptionalExpressionList:
  /* Empty. */   { $$ = new ListExpr (yyloc); }
| ExpressionList { $$ = $1; }

OptionalTypeOrExpressionList:
  /* Empty. */         { $$ = new ListExpr (yyloc); }
| TypeOrExpressionList { $$ = $1; }

ExpressionList:
  Expression
{ $$ = (new ListExpr (@1))->append ($1); }
| ExpressionList ',' Expression
{ $$ = $1->append ($3); }

TypeOrExpressionList:
  Expression
{ $$ = (new ListExpr (@1))->append ($1); }
| TypeLitExpression
{ $$ = (new ListExpr (@1))->append (new TypeExpression (@1, $1)); }
| TypeOrExpressionList ',' Expression
{ $$ = $1->append ($3); }
| TypeOrExpressionList ',' TypeLitExpression
{ $$ = $1->append (new TypeExpression (@1, $3)); }

ExpressionStatement:
  Expression ';'
{ $$ = new ExpressionStatement (@1, $1); }

VarStatement:
  VAR IdentifierList Mutability DereferenceMutability Type '=' ExpressionList ';'
{ $$ = new VarStatement (@1, $2, $3, $4, $5, $7); }
| VAR IdentifierList Mutability DereferenceMutability Type ';'
{ $$ = new VarStatement (@1, $2, $3, $4, $5, new ListExpr (@1)); }
| VAR IdentifierList Mutability DereferenceMutability '=' ExpressionList ';'
{ $$ = new VarStatement (@1, $2, $3, $4, new EmptyTypeSpec (@1), $6); }
| IdentifierList Mutability DereferenceMutability SHORT_ASSIGN ExpressionList ';'
{ $$ = new VarStatement (@1, $1, $2, $3, new EmptyTypeSpec (@1), $5); }

AssignmentStatement:
  Expression '=' Expression ';'
{ $$ = new AssignStatement (@1, $1, $3); }
| Expression ADD_ASSIGN Expression ';'
{ $$ = new AddAssignStatement (@1, $1, $3); }

IfStatement:
  IF Expression Block
  { $$ = new IfStatement (@1, new EmptyStatement (@1), $2, $3, new ListStatement (@1)); }
| IF Expression Block ELSE IfStatement
{ $$ = new IfStatement (@1, new EmptyStatement (@1), $2, $3, $5); }
| IF Expression Block ELSE Block
{ $$ = new IfStatement (@1, new EmptyStatement (@1), $2, $3, $5); }
| IF SimpleStatement ';' Expression Block
{ $$ = new IfStatement (@1, $2, $4, $5, new ListStatement (@1)); }
| IF SimpleStatement ';' Expression Block ELSE IfStatement
{ $$ = new IfStatement (@1, $2, $4, $5, $7); }
| IF SimpleStatement ';' Expression Block ELSE Block
{ $$ = new IfStatement (@1, $2, $4, $5, $7); }

WhileStatement:
  FOR Expression Block
{ $$ = new WhileStatement (@1, $2, $3); }

IdentifierList:
  IDENTIFIER
{ $$ = (new IdentifierList (@1))->append ($1); }
| IdentifierList ',' IDENTIFIER
{ $$ = $1->append ($3); }

// Type literals that can appear in expressions.
TypeLitExpression:
  ArrayType     { $$ = $1; }
| StructType    { $$ = $1; }
| SliceType     { $$ = $1; }
| MapType       { $$ = $1; }
| ComponentType { $$ = $1; }
| PushPortType  { $$ = $1; }
| PullPortType  { $$ = $1; }
| HeapType      { $$ = $1; }

ArrayType:
  ArrayDimension ElementType { $$ = new ArrayTypeSpec (@1, $1, $2); }

ElementType: Type { $$ = $1; }

StructType: STRUCT '{' FieldList '}' { $$ = $3; }

SliceType: '[' ']' ElementType { $$ = new SliceTypeSpec (@1, $3); }

MapType: MAP '[' KeyType ']' ElementType { $$ = new MapTypeSpec (@1, $3, $5); }

KeyType: Type { $$ = $1; }

ComponentType:
  COMPONENT '{' FieldList '}'
{ $$ = $3; static_cast<FieldListTypeSpec*> ($3)->is_component = true; }

PushPortType: PUSH ParameterList { $$ = new PushPortTypeSpec (@1, $2); }

PullPortType:
  PULL ParameterList ReturnParameterList
{ $$ = new PullPortTypeSpec (@1, $2, $3); }

HeapType: HEAP Type { $$ = new HeapTypeSpec (@1, $2); }

PointerType: '*' Type { $$ = new PointerTypeSpec (@1, $2); }

TypeLit:
  PointerType       { $$ = $1; }
| TypeLitExpression { $$ = $1; }

TypeName: IDENTIFIER { $$ = new IdentifierTypeSpec (@1, $1); }

Type:
  TypeName     { $$ = $1; }
| TypeLit      { $$ = $1; }
| '(' Type ')' { $$ = $2; }

ArrayDimension: '[' Expression ']' { $$ = $2; }

FieldList:
  /* empty */
{ $$ = new FieldListTypeSpec (yyloc); }
| FieldList IdentifierList Type ';'
{ $$ = $1->append (new IdentifierListTypeSpec (@1, $2, Mutable, Mutable, $3)); }

OptionalExpression:
  /* empty */
{ $$ = new EmptyExpr (yyloc); }
| Expression
{ $$ = $1; }

Expression: OrExpression { $$ = $1; }

OrExpression:
  AndExpression
{ $$ = $1; }
| OrExpression LOGIC_OR AndExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::logic_or_temp, $1, $3); }

AndExpression:
  CompareExpression
{ $$ = $1; }
| AndExpression LOGIC_AND CompareExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::logic_and_temp, $1, $3); }

CompareExpression:
  AddExpression
{ $$ = $1; }
| CompareExpression EQUAL AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::equal_temp, $1, $3); }
| CompareExpression NOT_EQUAL AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::not_equal_temp, $1, $3); }
| CompareExpression '<' AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::less_than_temp, $1, $3); }
| CompareExpression LESS_EQUAL AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::less_equal_temp, $1, $3); }
| CompareExpression '>' AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::more_than_temp, $1, $3); }
| CompareExpression MORE_EQUAL AddExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::more_equal_temp, $1, $3); }

AddExpression:
  MultiplyExpression
{ $$ = $1; }
| AddExpression '+' MultiplyExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::add_temp, $1, $3); }
| AddExpression '-' MultiplyExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::subtract_temp, $1, $3); }
| AddExpression '|' MultiplyExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::bit_or_temp, $1, $3); }
| AddExpression '^' MultiplyExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::bit_xor_temp, $1, $3); }

MultiplyExpression:
  UnaryExpression
{ $$ = $1; }
| MultiplyExpression '*' UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::multiply_temp, $1, $3); }
| MultiplyExpression '/' UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::divide_temp, $1, $3); }
| MultiplyExpression '%' UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::modulus_temp, $1, $3); }
| MultiplyExpression LEFT_SHIFT UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::left_shift_temp, $1, $3); }
| MultiplyExpression RIGHT_SHIFT UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::right_shift_temp, $1, $3); }
| MultiplyExpression '&' UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::bit_and_temp, $1, $3); }
| MultiplyExpression AND_NOT UnaryExpression
{ $$ = new BinaryArithmeticExpr (@1, &semantic::bit_and_not_temp, $1, $3); }

UnaryExpression:
  PrimaryExpression   { $$ = $1; }
| '+' UnaryExpression { $$ = new UnaryArithmeticExpr (@1, &semantic::posate_temp, $2); }
| '-' UnaryExpression { $$ = new UnaryArithmeticExpr (@1, &semantic::negate_temp, $2); }
| '!' UnaryExpression { $$ = new UnaryArithmeticExpr (@1, &semantic::logic_not_temp, $2); }
| '^' UnaryExpression { $$ = new UnaryArithmeticExpr (@1, &semantic::complement_temp, $2); }
| '*' UnaryExpression { $$ = new DereferenceExpr (@1, $2); }
| '&' UnaryExpression { $$ = new AddressOfExpr (@1, $2); }

PrimaryExpression:
  LITERAL
{ $$ = $1; }
| TypeLitExpression LiteralValue
{ $$ = new CompositeLiteral (@1, $1, $2); }
| IDENTIFIER LiteralValue
{ $$ = new CompositeLiteral (@1, new IdentifierTypeSpec (@1, $1), $2); }
| '[' DOTDOTDOT ']' ElementType LiteralValue
{ $$ = new CompositeLiteral (@1, new ArrayTypeSpec (@1, new EmptyExpr (@1), $4), $5); }
/* | FunctionLit */
| IDENTIFIER
{ $$ = new IdentifierExpr (@1, $1); }
| '(' Expression ')'
{ $$ = $2; }
| PrimaryExpression '.' IDENTIFIER
{ $$ = new SelectExpr (@1, $1, $3); }
| PrimaryExpression '[' Expression ']'
{ $$ = new IndexExpr (@1, $1, $3); }
| PrimaryExpression '[' OptionalExpression ':' OptionalExpression ']'
{ $$ = new SliceExpr (@1, $1, $3, $5, new EmptyExpr (yyloc)); }
| PrimaryExpression '[' OptionalExpression ':' Expression ':' Expression ']'
{ $$ = new SliceExpr (@1, $1, $3, $5, $7); }
| PrimaryExpression '(' OptionalTypeOrExpressionList ')'
{ $$ = new CallExpr (@1, $1, $3); }
| TypeLitExpression '(' Expression ')'
{ $$ = new ConversionExpr (@1, new TypeExpression (@1, $1), $3); }

LiteralValue:
  '{' '}'                           { $$ = new ElementList (@1); }
| '{' ElementList OptionalComma '}' { $$ = $2;                          }

ElementList:
  Element                 { $$ = (new ElementList (@1))->append ($1); }
| ElementList ',' Element { $$ = $1->append ($3); }

Element:
  Key ':' Value { $$ = new Element (@1, $1, $3); }
| Value         { $$ = new Element (@1, new EmptyExpr (@1), $1); }

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
