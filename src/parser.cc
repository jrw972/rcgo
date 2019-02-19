// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/parser.h"

#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "src/ast.h"
#include "src/error_reporter.h"
#include "src/token.h"

// https://golang.org/ref/spec

namespace rcgo {

ast::Node* Parser::Identifier() {
  Token token = Peek();
  if (Want(Token::kIdentifier)) {
    return new ast::Identifier(token);
  } else {
    return new ast::Error(token.location());
  }
}

ast::Node* Parser::Type() {
  // Type              = TypeName | TypeLit | "(" Type ")" .
  // TypeLit           = ArrayType | StructType | PointerType | FunctionType |
  //                     InterfaceType | SliceType | MapType | ChannelType .
  // ArrayType         = "[" ArrayLength "]" ElementType .
  // ArrayLength       = Expression .
  // ElementType       = Type .
  // SliceType         = "[" "]" ElementType .
  // StructType        = "struct" "{" { FieldDecl ";" } "}" .
  // PointerType       = "*" BaseType .
  // BaseType          = Type .
  // FunctionType      = "func" Signature .
  // InterfaceType     = "interface" "{" { MethodSpec ";" } "}" .
  // MapType           = "map" "[" KeyType "]" ElementType .
  // KeyType           = Type .
  // ChannelType       = ( "chan" | "chan" "<-" | "<-" "chan" ) ElementType .
  //                     (NOT IMPLEMENTED)

  switch (Peek().kind()) {
    case Token::kIdentifier:
      {
        return TypeName();
      }

    case Token::kLeftParen:
      {
        Next();
        ast::Node* type = Type();
        Want(Token::kRightParen);
        return type;
      }

    case Token::kLeftBracket:
      {
        Location left_bracket_loc = Next().location();
        if (Got(Token::kRightBracket)) {
          return new ast::Slice(left_bracket_loc, Type());
        }
        ast::Node* capacity = NULL;
        if (!Got(Token::kEllipsis)) {
          ++m_expression_level;
          capacity = Expression();
          --m_expression_level;
        }
        Want(Token::kRightBracket);
        return new ast::Array(left_bracket_loc, capacity, Type());
      }

    case Token::kMap:
      {
        Location map_keyword_loc = Next().location();
        Want(Token::kLeftBracket);
        ast::Node* keyType = Type();
        Want(Token::kRightBracket);
        ast::Node* valueType = Type();
        return new ast::Map(map_keyword_loc, keyType, valueType);
      }

    case Token::kPointer:
      {
        Location star_token_loc = Next().location();
        return new ast::Pointer(star_token_loc, Type());
      }

    case Token::kFunc:
      return new ast::FunctionType(Next().location(), Signature());

    case Token::kStruct:
      {
        const Location loc = Peek().location();
        ast::ListType children;
        Next();
        Want(Token::kLeftBrace);
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightBrace) {
          ast::Node* fieldDecl = FieldDecl();
          if (fieldDecl != NULL) {
            children.push_back(fieldDecl);
            if (Peek() == Token::kRightBrace) {
              break;
            }
            Want(Token::kSemicolon);
            continue;
          }
          Advance(Token::kRightBrace, Token::kSemicolon, Token::kEnd);
          Got(Token::kSemicolon);
        }
        Want(Token::kRightBrace);
        return new ast::Struct(loc, children);
      }

    case Token::kInterface:
      {
        const Location loc = Peek().location();
        ast::ListType children;
        Next();
        Want(Token::kLeftBrace);
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightBrace) {
          ast::Node* methodSpec = MethodSpec();
          if (methodSpec != NULL) {
            children.push_back(methodSpec);
            if (Peek() == Token::kRightBrace) {
              break;
            }
            Want(Token::kSemicolon);
            continue;
          }
          Advance(Token::kRightBrace, Token::kSemicolon, Token::kEnd);
          Got(Token::kSemicolon);
        }
        Want(Token::kRightBrace);
        return new ast::Interface(loc, children);
      }

    default:
      {
        Location loc = Next().location();
        m_error_reporter->Insert(ExpectedAType(loc));
        return new ast::Error(loc);
      }
  }
}

ast::Node* Parser::TypeName() {
  // TypeName          = identifier | QualifiedIdent .
  return OptionalSelector(Identifier());
}

ast::Node* Parser::FieldDecl() {
  // FieldDecl         =(IdentifierList Type | EmbeddedField) [ Tag ] .
  // EmbeddedField    = [ "*" ] TypeName .
  // Tag               = string_lit .

  switch (Peek().kind()) {
    case Token::kPointer:
      {
        Location star_loc = Next().location();
        // EmbeddedField of form * TypeName
        ast::Node* typeName = TypeName();
        std::string tag;
        if (Peek() == Token::kLiteral &&
            Peek().value().IsString()) {
          tag = Peek().value().string_value();
          Next();
        }
        return new ast::EmbeddedField(star_loc, true, typeName, tag);
      }

    case Token::kIdentifier:
      {
        ast::Node* identifier = Identifier();
        if (Peek() == Token::kLiteral &&
            Peek().value().IsString()) {
          std::string tag = Peek().value().string_value();
          Next();
          return new ast::EmbeddedField(identifier->location, false, identifier,
                                        tag);
        }

        switch (Peek().kind()) {
          case Token::kSemicolon:  // lone identifier
          case Token::kPeriod:     // QualifiedIdent
          case Token::kRightBrace:
            {
              // At this point, we have a single identifier.
              // These tokens indicate an embedded field.
              ast::Node* typeName = OptionalSelector(identifier);
              std::string tag;
              if (Peek() == Token::kLiteral &&
                  Peek().value().IsString()) {
                tag = Peek().value().string_value();
                Next();
              }
              return new ast::EmbeddedField(identifier->location, false,
                                            typeName, tag);
            }

          default:
            {
              // IdentifierList Type
              ast::ListType identifierList = IdentifierList(identifier);
              ast::Node* type = Type();
              std::string tag;
              if (Peek() == Token::kLiteral &&
                  Peek().value().IsString()) {
                tag = Peek().value().string_value();
                Next();
              }
              return new ast::Field(identifier->location, identifierList, type,
                                    tag);
            }
        }
      }

    default:
      m_error_reporter->Insert(ExpectedAFieldDeclaration(Next().location()));
      return NULL;
  }
}

ast::Node* Parser::Signature() {
  // Signature         = Parameters [ Result ] .
  // Result            = Parameters | Type .
  const Location location = Peek().location();
  ast::ListType parameters = Parameters();
  switch (Peek().kind()) {
    case Token::kLeftParen:
      return new ast::Signature(location, parameters, Parameters());
    case Token::kIdentifier:
    case Token::kLeftBracket:
    case Token::kStruct:
    case Token::kPointer:
    case Token::kFunc:
    case Token::kInterface:
    case Token::kMap:
      {
        const Location loc = Peek().location();
        ast::ListType results;
        results.push_back(new ast::ParameterDecl(loc, ast::ListType(), false,
                                                 Type()));
        return new ast::Signature(location, parameters, results);
      }
    default:
      {
        // No result.
        return new ast::Signature(location, parameters, ast::ListType());
      }
  }
}

ast::ListType Parser::Parameters() {
  // Parameters        = "(" [ ParameterList [ "," ] ] ")" .
  // ParameterList     = ParameterDecl { "," ParameterDecl } .
  // ParameterDecl     = [ IdentifierList ] [ "..." ] Type .

  // Parse as types.

  Want(Token::kLeftParen);
  if (Got(Token::kRightParen)) {
    // Done.  Types with no identifiers.
    return ast::ListType();
  }

  const Location location = Peek().location();
  ast::ListType initialList;

  // Parse the IdentifierList as a list of Types.
  while (Peek() != Token::kRightParen) {
    initialList.push_back(Type());
    if (Got(Token::kComma)) {
      continue;
    }
    break;
  }

  if (Got(Token::kRightParen)) {
    // Done.  Types with no identifiers.
    ast::ListType children;
    for (ast::Node* node : initialList) {
      ast::Node* parameterDecl =
          new ast::ParameterDecl(node->location, ast::ListType(), false, node);
      children.push_back(parameterDecl);
    }
    return children;
  }

  // Check that initialList only contains identifiers.
  for (std::vector<ast::Node*>::const_iterator pos = initialList.begin(),
           limit = initialList.end();
       pos != limit;
       ++pos) {
    ast::Node* a = *pos;
    // TODO(jrw972):  Replace dynamic_casts with Cast.
    if (dynamic_cast<ast::Identifier*>(a) == NULL &&
        dynamic_cast<ast::Error*>(a) == NULL) {
      m_error_reporter->Insert(Expected1(a->location, Token::kIdentifier));
    }
  }

  ast::ListType identifierList(initialList);
  bool variadic = Got(Token::kEllipsis);
  ast::Node* type = Type();

  ast::ListType children;
  children.push_back(new ast::ParameterDecl(location, identifierList, variadic,
                                            type));

  while (Got(Token::kComma)) {
    if (Peek() == Token::kRightParen) {
      break;
    }

    const Location location = Peek().location();
    ast::ListType identifierList = IdentifierList();
    bool variadic = Got(Token::kEllipsis);
    ast::Node* type = Type();
    children.push_back(new ast::ParameterDecl(location, identifierList,
                                              variadic, type));
  }
  ast::ListType parameters(children);
  Want(Token::kRightParen);

  // Check for variadic only at end.
  for (size_t idx = 0, limit = parameters.size() - 1;
       idx != limit;
       ++idx) {
    ast::ParameterDecl* p =
        dynamic_cast<ast::ParameterDecl*>(parameters.at(idx));
    if (p->variadic) {
      m_error_reporter->Insert(IllegalVariadicParameter(p->location));
    }
  }

  return parameters;
}

ast::Node* Parser::MethodSpec() {
  // MethodSpec        = MethodName Signature | InterfaceTypeName .
  // MethodName        = identifier .
  // InterfaceTypeName = TypeName .

  if (Peek().kind() == Token::kIdentifier) {
    const Location location = Peek().location();
    ast::Node* identifier = Identifier();
    switch (Peek().kind()) {
      case Token::kSemicolon:  // lone identifier
      case Token::kPeriod:     // QualifiedIdent
      case Token::kRightBrace:
        return new ast::AnonymousMethodSpec(location,
                                            OptionalSelector(identifier));
      default:
        return new ast::MethodSpec(location, identifier, Signature());
    }
  }

  m_error_reporter->Insert(ExpectedAMethodSpecification(Next().location()));
  return NULL;
}

ast::Node* Parser::Operand() {
  // Operand     = Literal | OperandName | MethodExpr | "(" Expression ")" .
  // Literal     = BasicLit | CompositeLit | FunctionLit .
  // BasicLit    = int_lit | float_lit | imaginary_lit | rune_lit | string_lit .

  // FunctionLit = "func" Function .

  // NOTE:  MethodExpr looks like a Selector handled by PrimaryExpr.
  // NOTE:  CompositeLit handled by PrimaryExpr.

  // MethodExpr    = ReceiverType "." MethodName .
  // ReceiverType  = TypeName | "(" "*" TypeName ")" | "(" ReceiverType ")" .

  switch (Peek().kind()) {
    case Token::kIdentifier:
      return Identifier();

    case Token::kLeftParen:
      {
        Next();
        ++m_expression_level;
        ast::Node* operand = Expression();
        --m_expression_level;
        Want(Token::kRightParen);
        return operand;
      }

    case Token::kFunc:
      {
        ast::Node* type = new ast::FunctionType(Next().location(), Signature());
        if (Peek() == Token::kLeftBrace) {
          ++m_expression_level;
          ast::Node* body = Block();
          --m_expression_level;
          return new ast::FunctionLiteral(Next().location(), type, body);
        }
        return type;
      }

    case Token::kLeftBracket:
    case Token::kStruct:
    case Token::kPointer:
    case Token::kInterface:
    case Token::kMap:
      return Type();

    case Token::kLiteral:
      {
        Token token = Next();
        return new ast::Literal(token);
      }

    default:
      {
        Location loc = Next().location();
        m_error_reporter->Insert(ExpectedAnOperand(loc));
        return new ast::Error(loc);
      }
  }
}

ast::Node* Parser::Block() {
  // Block = "{" StatementList "}" .
  Location loc = Peek().location();
  Want(Token::kLeftBrace);
  ast::ListType statementList = StatementList();
  Want(Token::kRightBrace);
  return new ast::Block(loc, statementList);
}

ast::ListType Parser::StatementList() {
  // StatementList = { Statement ";" } .
  ast::ListType children;
  while (Peek() != Token::kEnd &&
         Peek() != Token::kRightBrace &&
         Peek() != Token::kDefault &&
         Peek() != Token::kCase) {
    ast::Node* statement = Statement();
    assert(statement != NULL);
    children.push_back(statement);
    if (Peek() == Token::kRightBrace) {
      break;
    }
    Want(Token::kSemicolon);
    continue;
  }
  return children;
}

ast::Node* Parser::Declaration() {
  // Declaration   = ConstDecl | TypeDecl | VarDecl .

  Token token = Peek();
  switch (token.kind()) {
    case Token::kConst:
      return ConstDecl();
    case Token::kType:
      return TypeDecl();
    case Token::kVar:
      return VarDecl();
    default:
      m_error_reporter->Insert(ExpectedADeclaration(token.location()));
      Next();
      return NULL;
  }
}

ast::Node* Parser::TopLevelDecl() {
  // TopLevelDecl  = Declaration | FunctionDecl | MethodDecl .

  Token token = Peek();
  switch (token.kind()) {
    case Token::kConst:
      return ConstDecl();

    case Token::kType:
      return TypeDecl();

    case Token::kVar:
      return VarDecl();

    case Token::kFunc:
      Next();
      if (Peek() == Token::kLeftParen) {
        return MethodDecl();
      } else {
        return FuncDecl();
      }

    default:
      m_error_reporter->Insert(ExpectedADeclaration(token.location()));
      Next();
      return NULL;
  }
}

ast::Node* Parser::ConstDecl() {
  // ConstDecl      = "const"( ConstSpec | "(" { ConstSpec ";" } ")" ) .

  const Location loc = Peek().location();
  ast::ListType children;

  Want(Token::kConst);
  switch (Peek().kind()) {
    case Token::kIdentifier:
      {
        children.push_back(ConstSpec());
        return new ast::ConstDecl(loc, children);
      }

    case Token::kLeftParen:
      {
        Next();
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightParen) {
          children.push_back(ConstSpec());
          if (!OptionalSemicolon(Token::kRightParen)) {
            break;
          }
        }
        Want(Token::kRightParen);
        return new ast::ConstDecl(loc, children);
      }

    default:
      m_error_reporter->Insert(
          ExpectedASpecificationOrGroup(Next().location()));
      return new ast::ConstDecl(loc, children);
  }
}

ast::Node* Parser::ConstSpec() {
  // ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .

  Token token = Peek();
  ast::ListType identifierList = IdentifierList();
  ast::Node* type = NULL;
  std::vector<ast::Node*> expressionList;

  switch (Peek().kind()) {
    case Token::kSemicolon:
    case Token::kRightParen:
      break;
    case Token::kAssign:
      Next();
      expressionList = ExpressionList();
      break;
    default:
      type = Type();
      Want(Token::kAssign);
      expressionList = ExpressionList();
      break;
  }
  if (!expressionList.empty() &&
      identifierList.size() != expressionList.size()) {
    m_error_reporter->Insert(
        ExpectedNExpressions(token.location(), identifierList.size()));
  }

  return new ast::ConstSpec(token.location(), identifierList, type,
                            expressionList);
}

ast::ListType Parser::IdentifierList(ast::Node* first) {
  // IdentifierList = identifier { "," identifier } .
  ast::ListType children;
  if (first != NULL) {
    children.push_back(first);
  } else {
    children.push_back(Identifier());
  }
  while (Got(Token::kComma)) {
    children.push_back(Identifier());
  }
  return children;
}

ast::ListType Parser::ExpressionList() {
  // ExpressionList = Expression { "," Expression } .
  ast::Node* expression = Expression();

  ast::ListType children;
  children.push_back(expression);

  while (Got(Token::kComma)) {
    if (Peek() == Token::kRightParen) {
      break;
    }
    children.push_back(Expression());
  }
  return children;
}

ast::Node* Parser::TypeDecl() {
  // TypeDecl     = "type"( TypeSpec | "(" { TypeSpec ";" } ")" ) .
  const Location loc = Peek().location();
  ast::ListType children;
  Want(Token::kType);
  switch (Peek().kind()) {
    case Token::kIdentifier:
      {
        children.push_back(TypeSpec());
        return new ast::TypeDecl(loc, children);
      }

    case Token::kLeftParen:
      {
        Next();
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightParen) {
          children.push_back(TypeSpec());
          if (!OptionalSemicolon(Token::kRightParen)) {
            break;
          }
        }
        Want(Token::kRightParen);
        return new ast::TypeDecl(loc, children);
      }

    default:
      m_error_reporter->Insert(
          ExpectedASpecificationOrGroup(Next().location()));
      return new ast::TypeDecl(loc, children);
  }
}

ast::Node* Parser::TypeSpec() {
  // TypeSpec     = AliasDecl | TypeDef .
  // AliasDecl    = identifier "=" Type .
  // TypeDef      = identifier Type .
  const Location location = Peek().location();
  ast::Node* identifier = Identifier();
  Token t = Peek();
  bool alias = Got(Token::kAssign);
  ast::Node* type = Type();
  return new ast::TypeSpec(location, identifier, type, alias);
}

ast::Node* Parser::VarDecl() {
  // VarDecl     = "var"( VarSpec | "(" { VarSpec ";" } ")" ) .
  const Location loc = Peek().location();
  ast::ListType children;
  Want(Token::kVar);
  switch (Peek().kind()) {
    case Token::kIdentifier:
      {
        children.push_back(VarSpec());
        return new ast::VarDecl(loc, children);
      }

    case Token::kLeftParen:
      {
        Next();
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightParen) {
          children.push_back(VarSpec());
          if (!OptionalSemicolon(Token::kRightParen)) {
            break;
          }
        }
        Want(Token::kRightParen);
        return new ast::VarDecl(loc, children);
      }

    default:
      m_error_reporter->Insert(
          ExpectedASpecificationOrGroup(Next().location()));
      return new ast::VarDecl(loc, children);
  }
}

ast::Node* Parser::VarSpec() {
  // VarSpec     = IdentifierList( Type [ "=" ExpressionList ] |
  //               "=" ExpressionList ) .

  const Location location = Peek().location();
  ast::ListType identifierList = IdentifierList();
  ast::Node* type = NULL;
  ast::ListType expressionList;

  switch (Peek().kind()) {
    case Token::kAssign:
      Next();
      expressionList = ExpressionList();
      break;
    default:
      type = Type();
      if (Got(Token::kAssign)) {
        expressionList = ExpressionList();
      }
      break;
  }

  return new ast::VarSpec(location, identifierList, type, expressionList);
}

ast::Node* Parser::FuncDecl() {
  // FunctionDecl = "func" FunctionName( Function | Signature ) .
  // FunctionName = identifier .
  // Function     = Signature FunctionBody .
  // FunctionBody = Block .

  const Location location = Peek().location();
  ast::Node* identifier = Identifier();
  ast::Node* signature = Signature();
  ast::Node* body = NULL;
  if (Peek() == Token::kLeftBrace) {
    body = Block();
  }
  return new ast::FuncDecl(location, identifier, signature, body);
}

ast::Node* Parser::MethodDecl() {
  // MethodDecl   = "func" Receiver MethodName( Function | Signature ) .
  // Receiver     = Parameters .

  const Location location = Peek().location();
  ast::ListType parameters = Parameters();
  ast::Node* identifier = Identifier();
  ast::Node* signature = Signature();
  ast::Node* body = NULL;
  if (Peek() == Token::kLeftBrace) {
    body = Block();
  }
  return new ast::MethodDecl(location, parameters, identifier, signature, body);
}

ast::Node* Parser::LiteralValue() {
  // LiteralValue  = "{" [ ElementList [ "," ] ] "}" .
  // ElementList   = KeyedElement { "," KeyedElement } .

  const Location loc = Peek().location();
  ast::ListType children;
  Want(Token::kLeftBrace);
  ++m_expression_level;
  if (Got(Token::kRightBrace)) {
    --m_expression_level;
    return new ast::LiteralValue(loc, children);
  }

  children.push_back(KeyedElement());
  while (Got(Token::kComma)) {
    if (Peek() == Token::kRightBrace) {
      break;
    }
    children.push_back(KeyedElement());
  }
  --m_expression_level;
  Want(Token::kRightBrace);

  return new ast::LiteralValue(loc, children);
}

ast::Node* Parser::KeyedElement() {
  // KeyedElement  = [ Key ":" ] Element .
  // Key           = FieldName | Expression | LiteralValue .
  // FieldName     = identifier .
  // Element       = Expression | LiteralValue .

  const Location location = Peek().location();
  ast::Node* key = NULL;
  switch (Peek().kind()) {
    case Token::kIdentifier:
      // potential FieldName
      key = Identifier();
      break;

    case Token::kLeftBrace:
      return new ast::KeyedElement(location, NULL, LiteralValue());

    default:
      key = Expression();
      break;
  }

  if (Got(Token::kColon)) {
    // Previous was key.
    ast::Node* element;
    if (Peek() == Token::kLeftBrace) {
      element = LiteralValue();
    } else {
      element = Expression();
    }
    return new ast::KeyedElement(location, key, element);
  } else {
    return new ast::KeyedElement(location, NULL, key);
  }
}

ast::Node* Parser::PrimaryExpr() {
  // PrimaryExpr =
  //    Operand |
  //    Conversion |
  //    PrimaryExpr Selector |
  //    PrimaryExpr Index |
  //    PrimaryExpr Slice |
  //    PrimaryExpr TypeAssertion |
  //    PrimaryExpr Arguments .

  // Selector       = "." identifier .
  // Index          = "[" Expression "]" .
  // Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
  //                  "[" [ Expression ] ":" Expression ":" Expression "]" .
  // TypeAssertion  = "." "(" Type ")" .
  // Arguments      = "(" [( ExpressionList |
  //                  Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .

  // Conversion = Type "(" Expression [ "," ] ")" .

  // CompositeLit  = LiteralType LiteralValue .
  // LiteralType   = StructType | ArrayType | "[" "..." "]" ElementType |
  //                 SliceType | MapType | TypeName .

  const Location location = Peek().location();
  ast::Node* operand = Operand();
  switch (Peek().kind()) {
    case Token::kPeriod:
    case Token::kLeftBracket:
    case Token::kLeftParen:
      return PrimaryExprSuffix(operand);

    case Token::kLeftBrace:
      // CompositeLit
      if ((dynamic_cast<ast::Identifier*>(operand) ||
           dynamic_cast<ast::Selector*>(operand)) &&
          m_expression_level >= 0) {
        return new ast::CompositeLiteral(location, operand, LiteralValue());
      } else if (dynamic_cast<ast::Array*>(operand) ||
                 dynamic_cast<ast::Slice*>(operand) ||
                 dynamic_cast<ast::Struct*>(operand) ||
                 dynamic_cast<ast::Map*>(operand)) {
        return new ast::CompositeLiteral(location, operand, LiteralValue());
      } else {
        return operand;
      }

    default:
      // Do nothing.
      return operand;
  }
}

ast::Node* Parser::PrimaryExprSuffix(ast::Node* operand) {
  switch (Peek().kind()) {
    case Token::kPeriod:
      {
        const Location loc = Peek().location();
        Next();
        if (Got(Token::kLeftParen)) {
          // TypeAssertion
          // Allow a type keyword.
          ast::Node* type;
          const Location loc = Peek().location();
          if (Got(Token::kType)) {
            type = new ast::TypeExpression(loc);
          } else {
            type = Type();
          }
          Want(Token::kRightParen);
          operand = new ast::TypeAssertion(loc, operand, type);
        } else {
          // Selector
          operand = new ast::Selector(loc, operand, Identifier());
        }
      }
      break;

    case Token::kLeftBracket:
      {
        // Index
        // Slice
        const Location loc = Peek().location();
        ++m_expression_level;
        Next();
        ast::Node* low = NULL;
        ast::Node* high = NULL;
        ast::Node* capacity = NULL;

        if (Peek() != Token::kColon) {
          low = Expression();
          if (Got(Token::kRightBracket)) {
            // [ Expression ]
            operand = new ast::Index(loc, operand, low);
            --m_expression_level;
            break;
          }
        }

        Want(Token::kColon);

        if (Got(Token::kRightBracket)) {
          // [ : ]
          // [ Expression : ]
          operand = new ast::SliceOp(loc, operand, low, high, capacity);
          --m_expression_level;
          break;
        }

        high = Expression();

        if (Got(Token::kRightBracket)) {
          // [ : Expression ]
          // [ Expression : Expression ]
          operand = new ast::SliceOp(loc, operand, low, high, capacity);
          --m_expression_level;
          break;
        }

        if (Got(Token::kColon)) {
          capacity = Expression();
          Want(Token::kRightBracket);
          // [ : Expression : Expression ]
          // [ Expression : Expression : Expression ]
          operand = new ast::SliceOp(loc, operand, low, high, capacity);
          --m_expression_level;
          break;
        }

        Next();
        m_error_reporter->Insert(InvalidSlice(loc));
        operand = new ast::SliceOp(loc, operand, low, high, capacity);
        --m_expression_level;
      }
      break;

    case Token::kLeftParen:
      {
        const Location loc = Peek().location();
        // Arguments
        Next();
        ++m_expression_level;
        if (Peek() != Token::kRightParen) {
          ast::ListType arguments = ExpressionList();
          bool variadic = false;
          if (Got(Token::kEllipsis)) {
            variadic = true;
          }
          Got(Token::kComma);
          operand = new ast::Call(loc, operand, arguments, variadic);
        } else {
          operand = new ast::Call(loc, operand, ast::ListType(), false);
        }
        --m_expression_level;
        Want(Token::kRightParen);
      }
      break;
    default:
      abort();  // Logic error. NOT_COVERED
      break;
  }

  switch (Peek().kind()) {
    case Token::kPeriod:
    case Token::kLeftBracket:
    case Token::kLeftParen:
      return PrimaryExprSuffix(operand);
      break;
    default:
      // Do nothing.
      return operand;
  }
}

ast::Node* Parser::OptionalSelector(ast::Node* operand) {
  const Location loc = Peek().location();
  if (Got(Token::kPeriod)) {
    return new ast::Selector(loc, operand, Identifier());
  }
  return operand;
}


ast::Node* Parser::Expression() {
  // Expression = UnaryExpr | Expression binary_op Expression .
  // binary_op  = "||" | "&&" | rel_op | add_op | mul_op .

  Location range_loc = Peek().location();
  if (Got(Token::kRange)) {
    return new ast::Range(range_loc, Expression());
  } else {
    return OrExpr();
  }
}

ast::Node* Parser::OrExpr() {
  const Location loc = Peek().location();
  ast::Node* operand = AndExpr();
  if (Peek() != Token::kLogicalOr) {
    return operand;
  }

  ast::ListType children;
  while (Got(Token::kLogicalOr)) {
    children.push_back(AndExpr());
  }

  return new ast::Or(loc, children);
}

ast::Node* Parser::AndExpr() {
  const Location loc = Peek().location();
  ast::Node* operand = RelExpr();
  if (Peek() != Token::kLogicalAnd) {
    return operand;
  }

  ast::ListType children;
  while (Got(Token::kLogicalAnd)) {
    children.push_back(RelExpr());
  }

  return new ast::And(loc, children);
}

ast::Node* Parser::RelExpr() {
  // rel_op     = "==" | "!=" | "<" | "<=" | ">" | ">=" .
  ast::Node* operand = AddExpr();
  while (Peek().IsRelOp()) {
    Token k = Peek();
    Next();
    switch (k.rel_op_kind()) {
      case Token::kEqual:
        return new ast::Equal(k.location(), operand, AddExpr());
      case Token::kNotEqual:
        return new ast::NotEqual(k.location(), operand, AddExpr());
      case Token::kLessThan:
        return new ast::LessThan(k.location(), operand, AddExpr());
      case Token::kLessThanEqual:
        return new ast::LessThanEqual(k.location(), operand, AddExpr());
      case Token::kGreaterThan:
        return new ast::GreaterThan(k.location(), operand, AddExpr());
      case Token::kGreaterThanEqual:
        return new ast::GreaterThanEqual(k.location(), operand, AddExpr());
    }
  }

  return operand;
}

ast::Node* Parser::AddExpr() {
  // add_op     = "+" | "-" | "|" | "^" .
  ast::Node* operand = MulExpr();
  while (Peek().IsAddOp()) {
    Token k = Peek();
    Next();
    switch (k.add_op_kind()) {
      case Token::kAdd:
        return new ast::Add(k.location(), operand, MulExpr());
      case Token::kSubtract:
        return new ast::Subtract(k.location(), operand, MulExpr());
      case Token::kBitOr:
        return new ast::BitOr(k.location(), operand, MulExpr());
      case Token::kBitXor:
        return new ast::BitXor(k.location(), operand, MulExpr());
    }
  }

  return operand;
}

ast::Node* Parser::MulExpr() {
  // mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
  ast::Node* operand = UnaryExpr();
  while (Peek().IsMulOp()) {
    Token k = Peek();
    Next();
    switch (k.mul_op_kind()) {
      case Token::kMultiply:
        return new ast::Multiply(k.location(), operand, UnaryExpr());
      case Token::kDivide:
        return new ast::Divide(k.location(), operand, UnaryExpr());
      case Token::kModulus:
        return new ast::Modulus(k.location(), operand, UnaryExpr());
      case Token::kLeftShift:
        return new ast::LeftShift(k.location(), operand, UnaryExpr());
      case Token::kRightShift:
        return new ast::RightShift(k.location(), operand, UnaryExpr());
      case Token::kBitAnd:
        return new ast::BitAnd(k.location(), operand, UnaryExpr());
      case Token::kBitAndNot:
        return new ast::BitAndNot(k.location(), operand, UnaryExpr());
    }
  }

  return operand;
}

ast::Node* Parser::UnaryExpr() {
  // UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .
  // unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
  // NOTE:  No "<-".
  if (Peek().IsUnaryOp()) {
    Token k = Peek();
    Next();
    switch (k.unary_op_kind()) {
      case Token::kPosate:
        return new ast::Posate(k.location(), UnaryExpr());
      case Token::kNegate:
        return new ast::Negate(k.location(), UnaryExpr());
      case Token::kLogicalNot:
        return new ast::LogicalNot(k.location(), UnaryExpr());
      case Token::kBitNot:
        return new ast::BitNot(k.location(), UnaryExpr());
      case Token::kIndirection:
        return new ast::Indirection(k.location(), UnaryExpr());
      case Token::kAddress:
        return new ast::Address(k.location(), UnaryExpr());
    }
  }

  return PrimaryExpr();
}

ast::Node* Parser::Statement() {
  // Statement =
  //    Declaration | LabeledStmt | SimpleStmt |
  //    GoStmt | ReturnStmt | BreakStmt | ContinueStmt | GotoStmt |
  //    FallthroughStmt | Block | IfStmt | SwitchStmt | SelectStmt | ForStmt |
  //    DeferStmt .

  // LabeledStmt = Label ":" Statement .
  // Label       = identifier .

  // GotoStmt = "goto" Label .

  // FallthroughStmt = "fallthrough" .

  // DeferStmt = "defer" Expression .

  // NOT IMPLEMENTED
  // Channel  = Expression .
  // GoStmt = "go" Expression .
  // SelectStmt = "select" "{" { CommClause } "}" .
  // CommClause = CommCase ":" StatementList .
  // CommCase   = "case"( SendStmt | RecvStmt ) | "default" .
  // RecvStmt   = [ ExpressionList "=" | IdentifierList ":=" ] RecvExpr .
  // RecvExpr   = Expression .

  if (Peek() == Token::kIdentifier && Peek(1) == Token::kColon) {
    const Location loc = Peek().location();
    ast::Node* identifier = Identifier();
    Want(Token::kColon);
    ast::Node* statement = Statement();
    assert(statement != NULL);
    return new ast::LabeledStatement(loc, identifier, statement);
  }

  switch (Peek().kind()) {
    case Token::kConst:
    case Token::kType:
    case Token::kVar:
      {
        return Declaration();
      }

    case Token::kReturn:
      return ReturnStmt();

    case Token::kBreak:
      return BreakStmt();

    case Token::kContinue:
      return ContinueStmt();

    case Token::kGoto:
      {
        Location goto_loc = Next().location();
        return new ast::Goto(goto_loc, Identifier());
      }

    case Token::kFallthrough:
      {
        Location fallthrough_loc = Next().location();
        return new ast::Fallthrough(fallthrough_loc);
      }

    case Token::kLeftBrace:
      return Block();

    case Token::kIf:
      return IfStmt();

    case Token::kSwitch:
      return SwitchStmt();

    case Token::kFor:
      return ForStmt();

    case Token::kDefer:
      {
        Location defer_loc = Next().location();
        return new ast::Defer(defer_loc, Expression());
      }

    default:
      return SimpleStmt();
  }
}

ast::Node* Parser::SimpleStmt() {
  // SimpleStmt = EmptyStmt | ExpressionStmt | SendStmt | IncDecStmt |
  //              Assignment | ShortVarDecl .
  // EmptyStmt = .
  // ExpressionStmt = Expression .
  // SendStmt = Channel "<-" Expression .(NOT IMPLEMENTED)
  // IncDecStmt = Expression( "++" | "--" ) .
  // Assignment = ExpressionList assign_op ExpressionList .
  // assign_op = [ add_op | mul_op ] "=" .
  // ShortVarDecl = IdentifierList ":=" ExpressionList .

  if (Peek() == Token::kSemicolon || Peek() == Token::kRightBrace) {
    // EmptyStmt
    return new ast::EmptyStatement(Peek().location());
  }

  ast::ListType expressionList = ExpressionList();
  const Location loc = Peek().location();
  if (Peek().IsAssignOp()) {
    if (expressionList.size() > 1) {
      m_error_reporter->Insert(UnexpectedExpressionList(loc));
    }
    Token k = Peek();
    Next();
    switch (k.assign_op_kind()) {
      case Token::kAddAssign:
        return new ast::AddAssign(k.location(), expressionList, Expression());
      case Token::kSubtractAssign:
        return new ast::SubtractAssign(k.location(), expressionList,
                                       Expression());
      case Token::kBitOrAssign:
        return new ast::BitOrAssign(k.location(), expressionList, Expression());
      case Token::kBitXorAssign:
        return new ast::BitXorAssign(k.location(), expressionList,
                                     Expression());
      case Token::kMultiplyAssign:
        return new ast::MultiplyAssign(k.location(), expressionList,
                                       Expression());
      case Token::kDivideAssign:
        return new ast::DivideAssign(k.location(), expressionList,
                                     Expression());
      case Token::kModulusAssign:
        return new ast::ModulusAssign(k.location(), expressionList,
                                      Expression());
      case Token::kLeftShiftAssign:
        return new ast::LeftShiftAssign(k.location(), expressionList,
                                        Expression());
      case Token::kRightShiftAssign:
        return new ast::RightShiftAssign(k.location(), expressionList,
                                         Expression());
      case Token::kBitAndAssign:
        return new ast::BitAndAssign(k.location(), expressionList,
                                     Expression());
      case Token::kBitAndNotAssign:
        return new ast::BitAndNotAssign(k.location(), expressionList,
                                        Expression());
    }
  }

  switch (Peek().kind()) {
    case Token::kIncrement:
      if (expressionList.size() > 1) {
        m_error_reporter->Insert(UnexpectedExpressionList(loc));
      }
      Next();
      return new ast::Increment(loc, expressionList);

    case Token::kDecrement:
      if (expressionList.size() > 1) {
        m_error_reporter->Insert(UnexpectedExpressionList(loc));
      }
      Next();
      return new ast::Decrement(loc, expressionList);

    case Token::kAssign:
      Next();
      return new ast::Assign(loc, expressionList, ExpressionList());
    case Token::kShortAssign:
      Next();
      return new ast::ShortAssign(loc, expressionList, ExpressionList());

    default:
      // Do nothing.
      if (expressionList.size() > 1) {
        m_error_reporter->Insert(UnexpectedExpressionList(loc));
      }
      return new ast::ExpressionStatement(loc, expressionList);
  }
}

ast::Node* Parser::ReturnStmt() {
  // ReturnStmt = "return" [ ExpressionList ] .

  Location return_loc = Peek().location();
  Want(Token::kReturn);
  if (Peek() != Token::kSemicolon) {
    return new ast::Return(return_loc, ExpressionList());
  } else {
    return new ast::Return(return_loc, ast::ListType());
  }
}

ast::Node* Parser::BreakStmt() {
  // BreakStmt = "break" [ Label ] .

  Location break_loc = Peek().location();
  Want(Token::kBreak);
  if (Peek() == Token::kIdentifier) {
    return new ast::Break(break_loc, Identifier());
  } else {
    return new ast::Break(break_loc, NULL);
  }
}

ast::Node* Parser::ContinueStmt() {
  // ContinueStmt = "continue" [ Label ] .

  Location continue_loc = Peek().location();
  Want(Token::kContinue);
  if (Peek() == Token::kIdentifier) {
    return new ast::Continue(continue_loc, Identifier());
  } else {
    return new ast::Continue(continue_loc, NULL);
  }
}

ast::Node* Parser::IfStmt() {
  // IfStmt = "if" [ SimpleStmt ";" ] Expression Block [ "else"( IfStmt |
  //          Block ) ] .

  Location if_loc = Peek().location();
  Want(Token::kIf);
  int outer = m_expression_level;
  m_expression_level = -1;
  ast::Node* expression;
  ast::Node* statement = SimpleStmt();
  if (Got(Token::kSemicolon)) {
    expression = Expression();
  } else {
    // SimpleStmt was Expression.
    expression = statement;
    statement = NULL;
  }
  m_expression_level = outer;
  ast::Node* block = Block();
  if (Got(Token::kElse)) {
    if (Peek() == Token::kIf) {
      return new ast::If (if_loc, statement, expression, block, IfStmt());
    } else if (Peek() == Token::kLeftBrace) {
      return new ast::If (if_loc, statement, expression, block, Block());
    } else {
      m_error_reporter->Insert(
          Expected2(Next().location(), Token::kIf, Token::kLeftBrace));
      return new ast::If (if_loc, statement, expression, block,
                          new ast::Block(if_loc, ast::ListType()));
    }
  } else {
    return new ast::If (if_loc, statement, expression, block,
                        new ast::Block(if_loc, ast::ListType()));
  }
}

ast::Node* Parser::SwitchStmt() {
  // SwitchStmt = ExprSwitchStmt | TypeSwitchStmt .
  // ExprSwitchStmt = "switch" [ SimpleStmt ";" ] [ Expression ]
  //                  "{" { ExprCaseClause } "}" .

  // TypeSwitchStmt  = "switch" [ SimpleStmt ";" ] TypeSwitchGuard
  //                   "{" { TypeCaseClause } "}" .
  // TypeSwitchGuard = [ identifier ":=" ] PrimaryExpr "." "(" "type" ")" .

  Location switch_loc = Peek().location();
  Want(Token::kSwitch);
  if (Peek() == Token::kLeftBrace) {
    return new ast::Switch (switch_loc, NULL, NULL, SwitchBody());
  }

  int outer = m_expression_level;
  m_expression_level = -1;
  ast::Node* statement = SimpleStmt();
  if (Peek() == Token::kLeftBrace) {
    // SimpleStmt was actually our Expression of TypeSwitchGuard.
    m_expression_level = outer;
    return new ast::Switch (switch_loc, NULL, statement, SwitchBody());
  }

  if (Got(Token::kSemicolon)) {
    if (Peek() == Token::kLeftBrace) {
      m_expression_level = outer;
      return new ast::Switch (switch_loc, statement, NULL, SwitchBody());
    } else {
      ast::Node* expression = Expression();
      m_expression_level = outer;
      return new ast::Switch (switch_loc, statement, expression, SwitchBody());
    }
  }

  m_expression_level = outer;

  m_error_reporter->Insert(
      Expected2(Next().location(), Token::kLeftBrace, Token::kSemicolon));
  return new ast::Switch (switch_loc, NULL, statement, ast::ListType());
}

ast::ListType Parser::SwitchBody() {
  // ExprCaseClause = ExprSwitchCase ":" StatementList .
  // ExprSwitchCase = "case" ExpressionList | "default" .
  // TypeCaseClause  = TypeSwitchCase ":" StatementList .
  // TypeSwitchCase  = "case" TypeList | "default" .
  // TypeList        = Type { "," Type } .

  ast::ListType children;

  Want(Token::kLeftBrace);
  while (Peek() != Token::kEnd &&
         Peek() != Token::kRightBrace) {
    Location loc = Peek().location();
    ast::ListType expressionList;
    if (!Got(Token::kDefault)) {
      Want(Token::kCase);
      expressionList = ExpressionList();
    }
    Want(Token::kColon);
    children.push_back(new ast::Case(loc, expressionList, StatementList()));
  }
  Want(Token::kRightBrace);

  return children;
}

ast::Node* Parser::ForStmt() {
  // ForStmt = "for" [ Condition | ForClause | RangeClause ] Block .
  // Condition = Expression .

  // ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
  // InitStmt = SimpleStmt .
  // PostStmt = SimpleStmt .
  // RangeClause = [ ExpressionList "=" | IdentifierList ":=" ]
  //               "range" Expression .

  // for Block
  // for Expression Block
  // for [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] Block
  // for [ InitStmt ] ";" [ Condition ] ";"  PostStmt  Block
  // for [ InitStmt ] ";"  Condition  ";" [ PostStmt ] Block
  // for [ InitStmt ] ";"  Condition  ";"  PostStmt  Block
  // for  InitStmt  ";" [ Condition ] ";" [ PostStmt ] Block
  // for  InitStmt  ";" [ Condition ] ";"  PostStmt  Block
  // for  InitStmt  ";"  Condition  ";" [ PostStmt ] Block
  // for  InitStmt  ";"  Condition  ";"  PostStmt  Block
  // for "range" Expression Block
  // for ExpressionList "=" "range" Expression Block
  // for IdentifierList ":=" "range" Expression Block

  Location for_loc = Peek().location();
  Want(Token::kFor);

  if (Peek() == Token::kLeftBrace) {
    // No condition.
    ast::Node* forClause = new ast::ForClause(for_loc, NULL, NULL, NULL);
    return new ast::For (for_loc, forClause, Block());
  }

  ast::Node* initStmt;
  ast::Node* condition;
  ast::Node* postStmt;

  int outer = m_expression_level;
  m_expression_level = -1;

  // InitStmt
  if (Peek() == Token::kSemicolon) {
    // Not there.
    initStmt = NULL;
  } else {
    initStmt = SimpleStmt();

    if (Peek() == Token::kLeftBrace) {
      // Condition.
      ast::Node* forClause = new ast::ForClause(for_loc, NULL, initStmt, NULL);
      m_expression_level = outer;
      return new ast::For (for_loc, forClause, Block());
    }
  }

  Want(Token::kSemicolon);
  // Condition
  if (Peek() == Token::kSemicolon) {
    // Not there.
    condition = NULL;
  } else {
    condition = Expression();
  }
  Want(Token::kSemicolon);
  // PostStmt
  if (Peek() == Token::kLeftBrace) {
    // Not there.
    postStmt = NULL;
  } else {
    postStmt = SimpleStmt();
  }
  ast::Node* forClause =
      new ast::ForClause(for_loc, initStmt, condition, postStmt);
  m_expression_level = outer;
  return new ast::For (for_loc, forClause, Block());
}

ast::SourceFile* Parser::SourceFile() {
  // SourceFile       = PackageClause ";" { ImportDecl ";" }
  //                    { TopLevelDecl ";" } .
  // PackageClause  = "package" PackageName .
  // PackageName    = identifier .

  Location package_loc = Peek().location();
  Want(Token::kPackage);
  ast::Node* package = Identifier();
  Want(Token::kSemicolon);
  Location loc = Peek().location();
  ast::ListType imports;
  while (Peek() == Token::kImport) {
    imports.push_back(ImportDecl());
    Want(Token::kSemicolon);
  }
  loc = Peek().location();
  ast::ListType decls;
  while (Peek() != Token::kEnd) {
    ast::Node* topLevelDecl = TopLevelDecl();
    if (topLevelDecl != NULL) {
      decls.push_back(topLevelDecl);
      Want(Token::kSemicolon);
    }
    Advance(Token::kConst, Token::kType, Token::kVar, Token::kFunc,
            Token::kEnd);
  }
  Want(Token::kEnd);
  return new ast::SourceFile(package_loc, package, imports, decls);
}

ast::Node* Parser::SourceFileHeader() {
  // SourceFile       = PackageClause ";" { ImportDecl ";" } .
  // PackageClause  = "package" PackageName .
  // PackageName    = identifier .

  Location package_loc = Peek().location();
  Want(Token::kPackage);
  ast::Node* package = Identifier();
  Want(Token::kSemicolon);
  ast::ListType imports;
  while (Peek() == Token::kImport) {
    imports.push_back(ImportDecl());
    Want(Token::kSemicolon);
  }
  return new ast::SourceFile(package_loc, package, imports, ast::ListType());
}

ast::Node* Parser::ImportDecl() {
  // ImportDecl       = "import"( ImportSpec | "(" { ImportSpec ";" } ")" ) .
  const Location loc = Peek().location();
  ast::ListType children;
  Want(Token::kImport);

  if (Peek().kind() == Token::kLiteral &&
      Peek().value().IsString()) {
    children.push_back(ImportSpec());
    return new ast::ImportDecl(loc, children);
  }

  switch (Peek().kind()) {
    case Token::kPeriod:
    case Token::kIdentifier:
      {
        children.push_back(ImportSpec());
        return new ast::ImportDecl(loc, children);
      }

    case Token::kLeftParen:
      {
        Next();
        while (Peek() != Token::kEnd &&
               Peek() != Token::kRightParen) {
          children.push_back(ImportSpec());
          if (!OptionalSemicolon(Token::kRightParen)) {
            break;
          }
        }
        Want(Token::kRightParen);
        return new ast::ImportDecl(loc, children);
      }

    default:
      m_error_reporter->Insert(
          ExpectedASpecificationOrGroup(Next().location()));
      return new ast::ImportDecl(loc, children);
  }
}

ast::Node* Parser::ImportSpec() {
  // ImportSpec       = [ "." | PackageName ] ImportPath .
  // ImportPath       = string_lit .

  Location loc = Peek().location();
  bool dot = false;
  ast::Node* identifier = NULL;
  if (Got(Token::kPeriod)) {
    dot = true;
  } else if (Peek() == Token::kIdentifier) {
    identifier = Identifier();
  }

  if (Peek().kind() == Token::kLiteral &&
      Peek().value().IsString()) {
    std::string path = Peek().value().string_value();
    Next();
    return new ast::ImportSpec(loc, dot, identifier, path);
  } else {
    return new ast::ImportSpec(loc, dot, identifier, "");
  }
}

void Parser::trace(bool t) {
  m_trace = t;
}

bool Parser::trace() const {
  return m_trace;
}

Token Parser::Peek(size_t offset) {
  return m_scanner.Peek(offset);
}

bool Parser::Got(Token::Kind kind) {
  if (Peek() == kind) {
    Next();
    return true;
  }
  return false;
}

Token Parser::Next() {
  if (m_trace) {
    m_out << "Consumed " << Peek() << std::endl;
  }
  return m_scanner.Consume();
}

bool Parser::Want(Token::Kind kind) {
  if (!Got(kind)) {
    m_error_reporter->Insert(Expected1(Peek().location(), kind));
    Next();
    return false;
  }
  return true;
}

void Parser::Advance(Token::Kind kind, ...) {
  assert(kind != Token::kEnd);

  std::set<Token::Kind> kinds;
  kinds.insert(kind);

  va_list args;
  va_start(args, kind);
  do {
    kind = static_cast<Token::Kind>(va_arg(args, int));
    kinds.insert(kind);
  } while (kind != Token::kEnd);
  va_end(args);

  assert(kinds.count(Token::kEnd) != 0);

  while (kinds.count(Peek().kind()) == 0) {
    Next();
  }
}

bool Parser::OptionalSemicolon(Token::Kind follow) {
  switch (Peek().kind()) {
    case Token::kSemicolon:
      Next();
      return true;
    case Token::kRightParen:
    case Token::kRightBrace:
      return true;
    default:
      m_error_reporter->Insert(Expected1(Peek().location(), follow));
      Advance(follow, Token::kEnd);
      return false;
  }
}

}  // namespace rcgo
