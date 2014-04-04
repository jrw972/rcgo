/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Parser.hpp"

#include <cstdarg>
#include <cstdlib>
#include <sstream>

#include "Ast.hpp"
#include "Token.hpp"
#include "Error.hpp"
#include "functions.hpp"

// https://golang.org/ref/spec

namespace rcgo {

  Parser::Parser (ByteStream& a_byte_stream,
                  ErrorReporter& a_error_reporter,
                  std::ostream& a_out)
    : m_scanner (a_byte_stream, a_error_reporter)
    , m_error_reporter (a_error_reporter)
    , m_out (a_out)
    , m_trace (false)
    , m_expression_level (0)
  { }

  Parser::~Parser ()
  { }

  Ast* Parser::Identifier ()
  {
    Token token = peek ();
    if (want (Token::Identifier))
      {
        return new IdentifierAst (token);
      }
    else
      {
        return new ErrorAst (token.location ());
      }
  }

  Ast* Parser::Type ()
  {
    // Type              = TypeName | TypeLit | "(" Type ")" .
    // TypeLit           = ArrayType | StructType | PointerType | FunctionType | InterfaceType | SliceType | MapType | ChannelType .
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
    // ChannelType       = ( "chan" | "chan" "<-" | "<-" "chan" ) ElementType . (NOT IMPLEMENTED)

    switch (peek ().kind ())
      {
      case Token::Identifier:
        {
          return TypeName ();
        }

      case Token::LeftParen:
        {
          next ();
          Ast* type = Type ();
          want (Token::RightParen);
          return type;
        }

      case Token::LeftBracket:
        {
          Location left_bracket_loc = next ().location ();
          if (got (Token::RightBracket))
            {
              return new SliceTypeAst (left_bracket_loc, Type ());
            }
          Ast* capacity = NULL;
          if (!got (Token::Ellipsis))
            {
              ++m_expression_level;
              capacity = Expression ();
              --m_expression_level;
            }
          want (Token::RightBracket);
          return new ArrayAst (left_bracket_loc, capacity, Type ());
        }

      case Token::Map:
        {
          Location map_keyword_loc = next ().location ();
          want (Token::LeftBracket);
          Ast* keyType = Type ();
          want (Token::RightBracket);
          Ast* valueType = Type ();
          return new MapAst (map_keyword_loc, keyType, valueType);
        }

      case Token::Pointer:
          {
            Location star_token_loc = next ().location ();
            return new PointerAst (star_token_loc, Type ());
          }

      case Token::Func:
        return new FunctionTypeAst (next ().location (), Signature ());

      case Token::Struct:
        {
          StructAst* type = new StructAst (next ().location ());
          want (Token::LeftBrace);
          while (peek () != Token::End &&
                 peek () != Token::RightBrace)
            {
              Ast* fieldDecl = FieldDecl ();
              if (fieldDecl != NULL)
                {
                  type->append (fieldDecl);
                  if (peek () == Token::RightBrace)
                    {
                      break;
                    }
                  want (Token::Semicolon);
                  continue;
                }
              advance (Token::RightBrace, Token::Semicolon, Token::End);
              got (Token::Semicolon);
            }
          want (Token::RightBrace);
          return type;
        }

      case Token::Interface:
        {
          InterfaceAst* type = new InterfaceAst (next ().location ());
          want (Token::LeftBrace);
          while (peek () != Token::End &&
                 peek () != Token::RightBrace)
            {
              Ast* methodSpec = MethodSpec ();
              if (methodSpec != NULL)
                {
                  type->append (methodSpec);
                  if (peek () == Token::RightBrace)
                    {
                      break;
                    }
                  want (Token::Semicolon);
                  continue;
                }
              advance (Token::RightBrace, Token::Semicolon, Token::End);
              got (Token::Semicolon);
            }
          want (Token::RightBrace);
          return type;
        }

      default:
        {
          Location loc = next ().location ();
          m_error_reporter << ExpectedAType (loc);
          return new ErrorAst (loc);
        }
      }

  }

  Ast* Parser::TypeName ()
  {
    // TypeName          = identifier | QualifiedIdent .
    return OptionalSelector (Identifier ());
  }

  Ast* Parser::FieldDecl ()
  {
    // FieldDecl         = (IdentifierList Type | EmbeddedField) [ Tag ] .
    // EmbeddedField    = [ "*" ] TypeName .
    // Tag               = string_lit .

    switch (peek ().kind ())
      {
      case Token::Pointer:
        {
          Location star_loc = next ().location ();
          // EmbeddedField of form * TypeName
          Ast* typeName = TypeName ();
          std::string tag;
          if (peek () == Token::Literal && peek ().value ().kind == Value::String)
            {
              tag = peek ().value ().String_value;
              next ();
            }
          return new EmbeddedFieldAst (star_loc, true, typeName, tag);
        }

      case Token::Identifier:
        {
          Location identifier_loc = peek ().location ();
          Ast* identifier = Identifier ();
          if (peek () == Token::Literal && peek ().value ().kind == Value::String)
            {
              std::string tag = peek ().value ().String_value;
              next ();
              return new EmbeddedFieldAst (identifier_loc, false, identifier, tag);
            }

          switch (peek ().kind ())
            {
            case Token::Semicolon: // lone identifier
            case Token::Period:    // QualifiedIdent
            case Token::RightBrace:
              {
                // At this point, we have a single identifier.
                // These tokens indicate an embedded field.
                Ast* typeName = OptionalSelector (identifier);
                std::string tag;
                if (peek () == Token::Literal && peek ().value ().kind == Value::String)
                  {
                    tag = peek ().value ().String_value;
                    next ();
                  }
                return new EmbeddedFieldAst (identifier_loc, false, typeName, tag);
              }

            default:
              {
                // IdentifierList Type
                Ast* identifierList = IdentifierList (identifier);
                Ast* type = Type ();
                std::string tag;
                if (peek () == Token::Literal && peek ().value ().kind == Value::String)
                  {
                    tag = peek ().value ().String_value;
                    next ();
                  }
                return new FieldAst (identifierList, type, tag);
              }
            }
        }

      default:
        m_error_reporter << ExpectedAFieldDeclaration (next ().location ());
        return NULL;
      }
  }
  
  Ast* Parser::Signature ()
  {
    // Signature         = Parameters [ Result ] .
    // Result            = Parameters | Type .

    Ast* parameters = Parameters ();
    switch (peek ().kind ())
      {
      case Token::LeftParen:
        return new SignatureAst (parameters, Parameters ());
      case Token::Identifier:
      case Token::LeftBracket:
      case Token::Struct:
      case Token::Pointer:
      case Token::Func:
      case Token::Interface:
      case Token::Map:
        {
          ListAst* results = new ListAst (peek ().location ());
          results->append (new ParameterDeclAst (NULL, false, Type()));
          return new SignatureAst (parameters, results);
        }
      default:
        {
          // No result.
          return new SignatureAst (parameters, new ListAst (parameters->location));
        }
      }
  }

  Ast* Parser::Parameters ()
  {
    // Parameters        = "(" [ ParameterList [ "," ] ] ")" .
    // ParameterList     = ParameterDecl { "," ParameterDecl } .
    // ParameterDecl     = [ IdentifierList ] [ "..." ] Type .

    // Parse as types.

    Location left_paren_loc = peek ().location ();
    want (Token::LeftParen);
    if (got (Token::RightParen))
      {
        // Done.  Types with no identifiers.
        return new ListAst (left_paren_loc);
      }

    std::vector<Ast*> initialList;

    // Parse the IdentifierList as a list of Types.
    while (peek () != Token::RightParen)
      {
        initialList.push_back (Type ());
        if (got (Token::Comma))
          {
            continue;
          }
        break;
      }

    if (got (Token::RightParen))
      {
        // Done.  Types with no identifiers.
        ListAst* parameters = new ListAst (left_paren_loc);
        for (std::vector<Ast*>::iterator pos = initialList.begin (),
               limit = initialList.end ();
             pos != limit;
             ++pos)
          {
            Ast* parameterDecl = new ParameterDeclAst (NULL, false, *pos);
            parameters->append (parameterDecl);
          }
        return parameters;
      }

    // Check that initialList only contains identifiers.
    for (std::vector<Ast*>::const_iterator pos = initialList.begin (),
           limit = initialList.end ();
         pos != limit;
         ++pos)
      {
        Ast* a = *pos;
        if (dynamic_cast<IdentifierAst*> (a) == NULL &&
            dynamic_cast<ErrorAst*> (a) == NULL)
          {
            m_error_reporter << Expected1 (a->location, Token::Identifier);
          }
      }

    ListAst* identifierList = new ListAst (initialList);
    bool variadic = got (Token::Ellipsis);
    Ast* type = Type ();

    ListAst* parameters = new ListAst (left_paren_loc);
    parameters->append (new ParameterDeclAst (identifierList, variadic, type));

    while (got (Token::Comma))
      {
        if (peek () == Token::RightParen)
          {
            break;
          }

        Ast* identifierList = IdentifierList ();
        bool variadic = got (Token::Ellipsis);
        Ast* type = Type ();
        parameters->append (new ParameterDeclAst (identifierList, variadic, type));
      }

    want (Token::RightParen);

    // Check for variadic only at end.
    for (size_t idx = 0, limit = parameters->size () - 1;
         idx != limit;
         ++idx)
      {
        ParameterDeclAst* p = dynamic_cast<ParameterDeclAst*> (parameters->at (idx));
        if (p->variadic)
          {
            m_error_reporter << IllegalVariadicParameter (p->location);
          }
      }

    return parameters;
  }

  Ast* Parser::MethodSpec ()
  {
    // MethodSpec        = MethodName Signature | InterfaceTypeName .
    // MethodName        = identifier .
    // InterfaceTypeName = TypeName .

    if (peek ().kind () == Token::Identifier)
      {
        Ast* identifier = Identifier ();
        switch (peek ().kind ())
          {
          case Token::Semicolon: // lone identifier
          case Token::Period:    // QualifiedIdent
          case Token::RightBrace:
            return new AnonymousMethodSpecAst (OptionalSelector (identifier));
          default:
            return new MethodSpecAst (identifier, Signature ());
          }
      }

    m_error_reporter << ExpectedAMethodSpecification (next ().location ());
    return NULL;
  }

  Ast* Parser::Operand ()
  {

    // Operand     = Literal | OperandName | MethodExpr | "(" Expression ")" .
    // Literal     = BasicLit | CompositeLit | FunctionLit .
    // BasicLit    = int_lit | float_lit | imaginary_lit | rune_lit | string_lit .

    // FunctionLit = "func" Function .

    // NOTE:  MethodExpr looks like a Selector handled by PrimaryExpr.
    // NOTE:  CompositeLit handled by PrimaryExpr.

    // MethodExpr    = ReceiverType "." MethodName .
    // ReceiverType  = TypeName | "(" "*" TypeName ")" | "(" ReceiverType ")" .

    switch (peek ().kind ())
      {
      case Token::Identifier:
        return Identifier ();

      case Token::LeftParen:
        {
          next ();
          ++m_expression_level;
          Ast* operand = Expression ();
          --m_expression_level;
          want (Token::RightParen);
          return operand;
        }

      case Token::Func:
        {
          Ast* type = new FunctionTypeAst (next ().location (), Signature ());
          if (peek () == Token::LeftBrace)
            {
              ++m_expression_level;
              Ast* body = Block ();
              --m_expression_level;
              return new FunctionLiteralAst (next ().location (), type, body);
            }
          return type;
        }

      case Token::LeftBracket:
      case Token::Struct:
      case Token::Pointer:
      case Token::Interface:
      case Token::Map:
        return Type ();

      case Token::Literal:
        {
          Token token = next ();
          return new LiteralAst (token);
        }

      default:
        {
          Location loc = next ().location ();
          m_error_reporter << ExpectedAnOperand (loc);
          return new ErrorAst (loc);
        }
      }
  }
  
  Ast* Parser::Block ()
  {
    // Block = "{" StatementList "}" .
    want (Token::LeftBrace);
    Ast* statementList = StatementList ();
    want (Token::RightBrace);
    return statementList;
  }

  Ast* Parser::StatementList ()
  {
    // StatementList = { Statement ";" } .
    Location loc = peek ().location ();
    ListAst* statementList = new ListAst (loc);
    while (peek () != Token::End &&
           peek () != Token::RightBrace &&
           peek () != Token::Default &&
           peek () != Token::Case)
      {
        Ast* statement = Statement ();
        assert (statement != NULL);
        statementList->append (statement);
        if (peek () == Token::RightBrace)
          {
            break;
          }
        want (Token::Semicolon);
        continue;
      }
    return statementList;
  }
  
  Ast* Parser::Declaration ()
  {
    // Declaration   = ConstDecl | TypeDecl | VarDecl .

    Token token = peek ();
    switch (token.kind ())
      {
      case Token::Const:
        return ConstDecl ();
      case Token::Type:
        return TypeDecl ();
      case Token::Var:
        return VarDecl ();
      default:
        m_error_reporter << ExpectedADeclaration (token.location ());
        next ();
        return NULL;
      }
  }

  Ast* Parser::TopLevelDecl ()
  {
    // TopLevelDecl  = Declaration | FunctionDecl | MethodDecl .

    Token token = peek ();
    switch (token.kind ())
      {
      case Token::Const:
        return ConstDecl ();

      case Token::Type:
        return TypeDecl ();

      case Token::Var:
        return VarDecl ();

      case Token::Func:
        next ();
        if (peek () == Token::LeftParen)
          {
            return MethodDecl ();
          }
        else
          {
            return FuncDecl ();
          }

      default:
        m_error_reporter << ExpectedADeclaration (token.location ());
        next ();
        return NULL;
      }
  }
  
  Ast* Parser::ConstDecl ()
  {
    // ConstDecl      = "const" ( ConstSpec | "(" { ConstSpec ";" } ")" ) .

    ListAst* constDecl = new ListAst (peek ().location ());

    want (Token::Const);
    switch (peek ().kind ())
      {
      case Token::Identifier:
        {
          constDecl->append (ConstSpec ());
          return constDecl;
        }

      case Token::LeftParen:
        {
          next ();
          while (peek () != Token::End &&
                 peek () != Token::RightParen)
            {
              constDecl->append (ConstSpec ());
              if (!optional_semicolon (Token::RightParen))
                {
                  break;
                }
            }
          want (Token::RightParen);
          return constDecl;
        }

      default:
        m_error_reporter << ExpectedASpecificationOrGroup (next ().location ());
        return constDecl;
      }
  }

  Ast* Parser::ConstSpec ()
  {
    // ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .

    Token token = peek ();
    Ast* identifierList = IdentifierList ();
    Ast* type = NULL;
    Ast* expressionList = NULL;

    switch (peek ().kind ())
      {
      case Token::Semicolon:
      case Token::RightParen:
        break;
      case Token::Assign:
        next ();
        expressionList = ExpressionList ();
        break;
      default:
        type = Type ();
        want (Token::Assign);
        expressionList = ExpressionList ();
        break;
      }
    if (expressionList != NULL &&  identifierList->size () != expressionList->size ())
      {
        m_error_reporter << ExpectedNExpressions (token.location (), identifierList->size ());
      }

    return new ConstSpecAst (token.location (), identifierList, type, expressionList);
  }
  
  Ast* Parser::IdentifierList (Ast* first)
  {
    // IdentifierList = identifier { "," identifier } .
    ListAst* identifierList = new ListAst (peek ().location ());
    if (first != NULL)
      {
        identifierList->append (first);
      }
    else
      {
        identifierList->append (Identifier ());
      }
    while (got (Token::Comma))
      {
        identifierList->append (Identifier ());
      }
    return identifierList;
  }

  Ast* Parser::ExpressionList (bool prefer_no_list)
  {
    // ExpressionList = Expression { "," Expression } .
    Ast* expression = Expression ();

    if (prefer_no_list && peek ().kind () != Token::Comma)
      {
        return expression;
      }

    ListAst* expressionList = new ListAst (expression);
    while (got (Token::Comma))
      {
        if (peek () == Token::RightParen)
          {
            break;
          }
        expressionList->append (Expression ());
      }
    return expressionList;
  }
  
  Ast* Parser::TypeDecl ()
  {
    // TypeDecl     = "type" ( TypeSpec | "(" { TypeSpec ";" } ")" ) .
    ListAst* typeDecl = new ListAst (peek ().location ());
    want (Token::Type);
    switch (peek ().kind ())
      {
      case Token::Identifier:
        {
          typeDecl->append (TypeSpec ());
          return typeDecl;
        }

      case Token::LeftParen:
        {
          next ();
          while (peek () != Token::End &&
                 peek () != Token::RightParen)
            {
              typeDecl->append (TypeSpec ());
              if (!optional_semicolon (Token::RightParen))
                {
                  break;
                }
            }
          want (Token::RightParen);
          return typeDecl;
        }

      default:
        m_error_reporter << ExpectedASpecificationOrGroup (next ().location ());
        return typeDecl;
      }
  }

  Ast* Parser::TypeSpec ()
  {
    // TypeSpec     = AliasDecl | TypeDef .
    // AliasDecl    = identifier "=" Type .
    // TypeDef      = identifier Type .
    Ast* identifier = Identifier ();
    Token t = peek ();
    bool alias = got (Token::Assign);
    Ast* type = Type ();
    return new TypeSpecAst (identifier, type, alias);
  }
  
  Ast* Parser::VarDecl ()
  {
    // VarDecl     = "var" ( VarSpec | "(" { VarSpec ";" } ")" ) .

    ListAst* varDecl = new ListAst (peek ().location ());
    want (Token::Var);
    switch (peek ().kind ())
      {
      case Token::Identifier:
        {
          varDecl->append (VarSpec ());
          return varDecl;
        }

      case Token::LeftParen:
        {
          next ();
          while (peek () != Token::End &&
                 peek () != Token::RightParen)
            {
              varDecl->append (VarSpec ());
              if (!optional_semicolon (Token::RightParen))
                {
                  break;
                }
            }
          want (Token::RightParen);
          return varDecl;
        }

      default:
        m_error_reporter << ExpectedASpecificationOrGroup (next ().location ());
        return varDecl;
      }
  }

  Ast* Parser::VarSpec ()
  {
    // VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .

    Ast* identifierList = IdentifierList ();
    Ast* type = NULL;
    Ast* expressionList = NULL;

    switch (peek ().kind ())
      {
      case Token::Assign:
        next ();
        expressionList = ExpressionList ();
        break;
      default:
        type = Type ();
        if (got (Token::Assign))
          {
            expressionList = ExpressionList ();
          }
        break;
      }

    return new VarSpecAst (identifierList, type, expressionList);
  }

  Ast* Parser::FuncDecl ()
  {
    // FunctionDecl = "func" FunctionName ( Function | Signature ) .
    // FunctionName = identifier .
    // Function     = Signature FunctionBody .
    // FunctionBody = Block .

    Ast* identifier = Identifier ();
    Ast* signature = Signature ();
    Ast* body = NULL;
    if (peek () == Token::LeftBrace)
      {
        body = Block ();
      }
    return new FuncDeclAst (identifier, signature, body);
  }

  Ast* Parser::MethodDecl ()
  {
    // MethodDecl   = "func" Receiver MethodName ( Function | Signature ) .
    // Receiver     = Parameters .

    Ast* parameters = Parameters ();
    Ast* identifier = Identifier ();
    Ast* signature = Signature ();
    Ast* body = NULL;
    if (peek () == Token::LeftBrace)
      {
        body = Block ();
      }
    return new MethodDeclAst (parameters, identifier, signature, body);
  }    

  Ast* Parser::LiteralValue ()
  {
    // LiteralValue  = "{" [ ElementList [ "," ] ] "}" .
    // ElementList   = KeyedElement { "," KeyedElement } .

    ListAst* literalValue = new ListAst (peek ().location ());
    want (Token::LeftBrace);
    ++m_expression_level;
    if (got (Token::RightBrace))
      {
        --m_expression_level;
        return literalValue;
      }

    literalValue->append (KeyedElement ());
    while (got (Token::Comma))
      {
        if (peek () == Token::RightBrace)
          {
            break;
          }
        literalValue->append (KeyedElement ());
      }
    --m_expression_level;
    want (Token::RightBrace);

    return literalValue;
  }

  Ast* Parser::KeyedElement ()
  {
    // KeyedElement  = [ Key ":" ] Element .
    // Key           = FieldName | Expression | LiteralValue .
    // FieldName     = identifier .
    // Element       = Expression | LiteralValue .

    Ast* key = NULL;
    switch (peek ().kind ())
      {
      case Token::Identifier:
        // potential FieldName
        key = Identifier ();
        break;

      case Token::LeftBrace:
        return new KeyedElementAst (NULL, LiteralValue ());

      default:
        key = Expression ();
        break;
      }
    
    if (got (Token::Colon))
      {
        // Previous was key.
        Ast* element;
        if (peek () == Token::LeftBrace)
          {
            element = LiteralValue ();
          }
        else
          {
            element = Expression ();
          }
        return new KeyedElementAst (key, element);
      }
    else
      {
        return new KeyedElementAst (NULL, key);
      }
  }

  Ast* Parser::PrimaryExpr ()
  {
    // PrimaryExpr =
    // 	Operand |
    // 	Conversion |
    // 	PrimaryExpr Selector |
    // 	PrimaryExpr Index |
    // 	PrimaryExpr Slice |
    // 	PrimaryExpr TypeAssertion |
    // 	PrimaryExpr Arguments .

    // Selector       = "." identifier .
    // Index          = "[" Expression "]" .
    // Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
    //                  "[" [ Expression ] ":" Expression ":" Expression "]" .
    // TypeAssertion  = "." "(" Type ")" .
    // Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .

    // Conversion = Type "(" Expression [ "," ] ")" .

    // CompositeLit  = LiteralType LiteralValue .
    // LiteralType   = StructType | ArrayType | "[" "..." "]" ElementType | SliceType | MapType | TypeName .

    Ast* operand = Operand ();
    switch (peek ().kind ())
      {
      case Token::Period:
      case Token::LeftBracket:
      case Token::LeftParen:
        return PrimaryExprSuffix (operand);

      case Token::LeftBrace:
        // CompositeLit
        if ((dynamic_cast<IdentifierAst*> (operand) ||
             dynamic_cast<SelectorAst*> (operand)) &&
            m_expression_level >= 0)
          {
            return new CompositeLiteralAst (operand, LiteralValue ());
          }
        else if (dynamic_cast<ArrayAst*> (operand) ||
                 dynamic_cast<SliceTypeAst*> (operand) ||
                 dynamic_cast<StructAst*> (operand) ||
                 dynamic_cast<MapAst*> (operand))
          {
            return new CompositeLiteralAst (operand, LiteralValue ());
          }
        else
          {
            return operand;
          }

      default:
        // Do nothing.
        return operand;
      }
  }

  Ast* Parser::PrimaryExprSuffix (Ast* operand)
  {
    switch (peek ().kind ())
      {
      case Token::Period:
        next ();
        if (got (Token::LeftParen))
          {
            // TypeAssertion
            // Allow a type keyword.
            Ast* type;
            Location t = peek ().location ();
            if (got (Token::Type))
              {
                type = new TypeExpressionAst (t);
              }
            else
              {
                type = Type ();
              }
            want (Token::RightParen);
            operand = new TypeAssertionAst (operand, type);
          }
        else
          {
            // Selector
            operand = new SelectorAst (operand, Identifier ());
          }
        break;

      case Token::LeftBracket:
        {
          // Index
          // Slice
          ++m_expression_level;
          next ();
          Ast* low = NULL;
          Ast* high = NULL;
          Ast* capacity = NULL;

          if (peek () != Token::Colon)
            {
              low = Expression ();
              if (got (Token::RightBracket))
                {
                  // [ Expression ]
                  operand = new IndexAst (operand, low);
                  --m_expression_level;
                  break;
                }
            }

          want (Token::Colon);

          if (got (Token::RightBracket))
            {
              // [ : ]
              // [ Expression : ]
              operand = new SliceAst (operand, low, high, capacity);
              --m_expression_level;
              break;
            }

          high = Expression ();

          if (got (Token::RightBracket))
            {
              // [ : Expression ]
              // [ Expression : Expression ]
              operand = new SliceAst (operand, low, high, capacity);
              --m_expression_level;
              break;
            }

          if (got (Token::Colon))
            {
              capacity = Expression ();
              want (Token::RightBracket);
              // [ : Expression : Expression ]
              // [ Expression : Expression : Expression ]
              operand = new SliceAst (operand, low, high, capacity);
              --m_expression_level;
              break;
            }

          Location loc = next ().location ();
          m_error_reporter << InvalidSlice (loc);
          operand = new SliceAst (operand, low, high, capacity);
          --m_expression_level;
        }
        break;

      case Token::LeftParen:
        {
          // Arguments
          Location left_paren_loc = next ().location ();
          ++m_expression_level;
          if (peek () != Token::RightParen)
            {
              Ast* arguments = ExpressionList ();
              bool variadic = false;
              if (got (Token::Ellipsis))
                {
                  variadic = true;
                }
              got (Token::Comma);
              operand = new CallAst (operand, arguments, variadic);
            }
          else
            {
              operand = new CallAst (operand, new ListAst (left_paren_loc), false);
            }
          --m_expression_level;
          want (Token::RightParen);
        }
        break;
      default:
        abort (); // Logic error. NOT_COVERED
        break;
      }

    switch (peek ().kind ())
      {
      case Token::Period:
      case Token::LeftBracket:
      case Token::LeftParen:
        return PrimaryExprSuffix (operand);
        break;
      default:
        // Do nothing.
        return operand;
      }
  }

  Ast* Parser::OptionalSelector (Ast* operand)
  {
    if (got (Token::Period))
      {
        return new SelectorAst (operand, Identifier ());
      }
    return operand;
  }


  Ast* Parser::Expression ()
  {
    // Expression = UnaryExpr | Expression binary_op Expression .
    // binary_op  = "||" | "&&" | rel_op | add_op | mul_op .

    Location range_loc = peek ().location ();
    if (got (Token::Range))
      {
        return new RangeAst (range_loc, Expression ());
      }
    else
      {
        return OrExpr ();
      }
  }

  Ast* Parser::OrExpr ()
  {
    Ast* operand = AndExpr ();
    if (peek () != Token::LogicalOr)
      {
        return operand;
      }

    OrAst* orAst = new OrAst (peek ().location ());
    while (got (Token::LogicalOr))
      {
        orAst->append (AndExpr ());
      }

    return orAst;
  }

  Ast* Parser::AndExpr ()
  {
    Ast* operand = RelExpr ();
    if (peek () != Token::LogicalAnd)
      {
        return operand;
      }

    AndAst* andAst = new AndAst (peek ().location ());
    while (got (Token::LogicalAnd))
      {
        andAst->append (RelExpr ());
      }

    return andAst;
  }

  Ast* Parser::RelExpr ()
  {
    // rel_op     = "==" | "!=" | "<" | "<=" | ">" | ">=" .
    Ast* operand = AddExpr ();
    while (peek ().is_rel_op ())
      {
        Token k = peek ();
        next ();
        BinaryAst::Kind kind;
        switch (k.rel_op_kind ())
          {
          case Token::Equal:
            kind = BinaryAst::Equal;
            break;
          case Token::NotEqual:
            kind = BinaryAst::NotEqual;
            break;
          case Token::LessThan:
            kind = BinaryAst::LessThan;
            break;
          case Token::LessThanEqual:
            kind = BinaryAst::LessThanEqual;
            break;
          case Token::GreaterThan:
            kind = BinaryAst::GreaterThan;
            break;
          case Token::GreaterThanEqual:
            kind = BinaryAst::GreaterThanEqual;
            break;
          }
        operand = new BinaryAst (kind, operand, AddExpr ());
      }

    return operand;
  }

  Ast* Parser::AddExpr ()
  {
    // add_op     = "+" | "-" | "|" | "^" .
    Ast* operand = MulExpr ();
    while (peek ().is_add_op ())
      {
        Token k = peek ();
        next ();
        BinaryAst::Kind kind;
        switch (k.add_op_kind ())
          {
          case Token::Add:
            kind = BinaryAst::Add;
            break;
          case Token::Subtract:
            kind = BinaryAst::Subtract;
            break;
          case Token::BitOr:
            kind = BinaryAst::BitOr;
            break;
          case Token::BitXor:
            kind = BinaryAst::BitXor;
            break;
          }
        operand = new BinaryAst (kind, operand, MulExpr ());
      }

    return operand;
  }

  Ast* Parser::MulExpr ()
  {
    // mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
    Ast* operand = UnaryExpr ();
    while (peek ().is_mul_op ())
      {
        Token k = peek ();
        next ();
        BinaryAst::Kind kind;
        switch (k.mul_op_kind ())
          {
          case Token::Multiply:
            kind = BinaryAst::Multiply;
            break;
          case Token::Divide:
            kind = BinaryAst::Divide;
            break;
          case Token::Modulus:
            kind = BinaryAst::Modulus;
            break;
          case Token::LeftShift:
            kind = BinaryAst::LeftShift;
            break;
          case Token::RightShift:
            kind = BinaryAst::RightShift;
            break;
          case Token::BitAnd:
            kind = BinaryAst::BitAnd;
            break;
          case Token::BitAndNot:
            kind = BinaryAst::BitAndNot;
            break;
          }
        operand = new BinaryAst (kind, operand, UnaryExpr ());
      }

    return operand;
  }

  Ast* Parser::UnaryExpr ()
  {
    // UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .
    // unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
    // NOTE:  No "<-".
    if (peek ().is_unary_op ())
      {
        Token k = peek ();
        next ();
        UnaryAst::Kind kind;
        switch (k.unary_op_kind ())
          {
          case Token::Posate:
            kind = UnaryAst::Posate;
            break;
          case Token::Negate:
            kind = UnaryAst::Negate;
            break;
          case Token::LogicalNot:
            kind = UnaryAst::LogicalNot;
            break;
          case Token::BitNot:
            kind = UnaryAst::BitNot;
            break;
          case Token::Indirection:
            kind = UnaryAst::Indirection;
            break;
          case Token::Address:
            kind = UnaryAst::Address;
            break;
          }
          return new UnaryAst (k.location (), kind, UnaryExpr ());
        }
    else
      {
        return PrimaryExpr ();
      }
  }
  
  Ast* Parser::Statement ()
  {
    // Statement =
    // 	Declaration | LabeledStmt | SimpleStmt |
    // 	GoStmt | ReturnStmt | BreakStmt | ContinueStmt | GotoStmt |
    // 	FallthroughStmt | Block | IfStmt | SwitchStmt | SelectStmt | ForStmt |
    // 	DeferStmt .
    
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
    // CommCase   = "case" ( SendStmt | RecvStmt ) | "default" .
    // RecvStmt   = [ ExpressionList "=" | IdentifierList ":=" ] RecvExpr .
    // RecvExpr   = Expression .

    if (peek () == Token::Identifier && peek (1) == Token::Colon)
      {
        Ast* identifier = Identifier ();
        want (Token::Colon);
        Ast* statement = Statement ();
        assert (statement != NULL);
        return new LabeledStatementAst (identifier, statement);
      }
    
    switch (peek ().kind ())
      {
      case Token::Const:
      case Token::Type:
      case Token::Var:
        {
          return Declaration ();
        }

      case Token::Return:
        return ReturnStmt ();

      case Token::Break:
        return BreakStmt ();

      case Token::Continue:
        return ContinueStmt ();

      case Token::Goto:
        {
          Location goto_loc = next ().location ();
          return new GotoAst (goto_loc, Identifier ());
        }

      case Token::Fallthrough:
        {
          Location fallthrough_loc = next ().location ();
          return new FallthroughAst (fallthrough_loc);
        }

      case Token::LeftBrace:
        return Block ();

      case Token::If:
        return IfStmt ();

      case Token::Switch:
        return SwitchStmt ();

      case Token::For:
        return ForStmt ();

      case Token::Defer:
        {
          Location defer_loc = next ().location ();
          return new DeferAst (defer_loc, Expression ());
        }

      default:
        return SimpleStmt ();
      }
  }

  Ast* Parser::SimpleStmt ()
  {
    // SimpleStmt = EmptyStmt | ExpressionStmt | SendStmt | IncDecStmt | Assignment | ShortVarDecl .
    // EmptyStmt = .
    // ExpressionStmt = Expression .
    // SendStmt = Channel "<-" Expression . (NOT IMPLEMENTED)
    // IncDecStmt = Expression ( "++" | "--" ) .
    // Assignment = ExpressionList assign_op ExpressionList .
    // assign_op = [ add_op | mul_op ] "=" .
    // ShortVarDecl = IdentifierList ":=" ExpressionList .

    if (peek () == Token::Semicolon || peek () == Token::RightBrace)
      {
        // EmptyStmt
        return new EmptyStatementAst (peek ().location ());
      }

    Ast* expressionList = ExpressionList (true);
    if (peek ().is_assign_op ())
      {
        if (dynamic_cast<ListAst*> (expressionList) == NULL)
          {
            expressionList = new ListAst (expressionList);
          }
        Token k = peek ();
        next ();
        AssignAst::Kind kind;
        switch (k.assign_op_kind ())
          {
          case Token::AddAssign:
            kind = AssignAst::Add;
            break;
          case Token::SubtractAssign:
            kind = AssignAst::Subtract;
            break;
          case Token::BitOrAssign:
            kind = AssignAst::BitOr;
            break;
          case Token::BitXorAssign:
            kind = AssignAst::BitXor;
            break;
          case Token::MultiplyAssign:
            kind = AssignAst::Multiply;
            break;
          case Token::DivideAssign:
            kind = AssignAst::Divide;
            break;
          case Token::ModulusAssign:
            kind = AssignAst::Modulus;
            break;
          case Token::LeftShiftAssign:
            kind = AssignAst::LeftShift;
            break;
          case Token::RightShiftAssign:
            kind = AssignAst::RightShift;
            break;
          case Token::BitAndAssign:
            kind = AssignAst::BitAnd;
            break;
          case Token::BitAndNotAssign:
            kind = AssignAst::BitAndNot;
            break;
          }
        return new AssignAst (kind, expressionList, ExpressionList ());
      }

    switch (peek ().kind ())
      {
      case Token::Increment:
        if (dynamic_cast<ListAst*> (expressionList) != NULL)
          {
            m_error_reporter << UnexpectedExpressionList (expressionList->location);
          }
        next ();
        return new IncrementAst (expressionList);

      case Token::Decrement:
        if (dynamic_cast<ListAst*> (expressionList) != NULL)
          {
            m_error_reporter << UnexpectedExpressionList (expressionList->location);
          }
        next ();
        return new DecrementAst (expressionList);

      case Token::Assign:
        next ();
        return new AssignAst (AssignAst::Assign, expressionList, ExpressionList ());
      case Token::ShortAssign:
        next ();
        return new AssignAst (AssignAst::ShortAssign, expressionList, ExpressionList ());

      default:
        // Do nothing.
        if (dynamic_cast<ListAst*> (expressionList) != NULL)
          {
            m_error_reporter << UnexpectedExpressionList (expressionList->location);
          }
        return new ExpressionStatementAst (expressionList);
      }    
  }
  
  Ast* Parser::ReturnStmt ()
  {
    // ReturnStmt = "return" [ ExpressionList ] .

    Location return_loc = peek ().location ();
    want (Token::Return);
    if (peek () != Token::Semicolon)
      {
        return new ReturnAst (return_loc, ExpressionList ());
      }
    else
      {
        return new ReturnAst (return_loc, NULL);
      }
  }

  Ast* Parser::BreakStmt ()
  {
    // BreakStmt = "break" [ Label ] .

    Location break_loc = peek ().location ();
    want (Token::Break);
    if (peek () == Token::Identifier)
      {
        return new BreakAst (break_loc, Identifier ());
      }
    else
      {
        return new BreakAst (break_loc, NULL);
      }
  }

  Ast* Parser::ContinueStmt ()
  {
    // ContinueStmt = "continue" [ Label ] .

    Location continue_loc = peek ().location ();
    want (Token::Continue);
    if (peek () == Token::Identifier)
      {
        return new ContinueAst (continue_loc, Identifier ());
      }
    else
      {
        return new ContinueAst (continue_loc, NULL);
      }
  }

  Ast* Parser::IfStmt ()
  {
    // IfStmt = "if" [ SimpleStmt ";" ] Expression Block [ "else" ( IfStmt | Block ) ] .

    Location if_loc = peek ().location ();
    want (Token::If);
    int outer = m_expression_level;
    m_expression_level = -1;
    Ast* expression;
    Ast* statement = SimpleStmt ();
    if (got (Token::Semicolon))
      {
        expression = Expression ();
      }
    else
      {
        // SimpleStmt was Expression.
        expression = statement;
        statement = NULL;
      }
    m_expression_level = outer;
    Ast* block = Block ();
    if (got (Token::Else))
      {
        if (peek () == Token::If)
          {
            return new IfAst (if_loc, statement, expression, block, IfStmt ());
          }
        else if (peek () == Token::LeftBrace)
          {
            return new IfAst (if_loc, statement, expression, block, Block ());
          }
        else
          {
            m_error_reporter << Expected2 (next ().location (), Token::If, Token::LeftBrace);
            return new IfAst (if_loc, statement, expression, block, new ListAst (if_loc));
          }
      }
    else
      {
        return new IfAst (if_loc, statement, expression, block, new ListAst (if_loc));
      }
  }

  Ast* Parser::SwitchStmt ()
  {
    // SwitchStmt = ExprSwitchStmt | TypeSwitchStmt .
    // ExprSwitchStmt = "switch" [ SimpleStmt ";" ] [ Expression ] "{" { ExprCaseClause } "}" .

    // TypeSwitchStmt  = "switch" [ SimpleStmt ";" ] TypeSwitchGuard "{" { TypeCaseClause } "}" .
    // TypeSwitchGuard = [ identifier ":=" ] PrimaryExpr "." "(" "type" ")" .

    Location switch_loc = peek ().location ();
    want (Token::Switch);
    if (peek () == Token::LeftBrace)
      {
        return new SwitchAst (switch_loc, NULL, NULL, SwitchBody ());
      }

    int outer = m_expression_level;
    m_expression_level = -1;
    Ast* statement = SimpleStmt ();
    if (peek () == Token::LeftBrace)
      {
        // SimpleStmt was actually our Expression of TypeSwitchGuard.
        m_expression_level = outer;
        return new SwitchAst (switch_loc, NULL, statement, SwitchBody ());
      }

    if (got (Token::Semicolon))
      {
        if (peek () == Token::LeftBrace)
          {
            m_expression_level = outer;
            return new SwitchAst (switch_loc, statement, NULL, SwitchBody ());
          }
        else
          {
            Ast* expression = Expression ();
            m_expression_level = outer;
            return new SwitchAst (switch_loc, statement, expression, SwitchBody ());
          }
      }

    m_expression_level = outer;

    m_error_reporter << Expected2 (next ().location (), Token::LeftBrace, Token::Semicolon);
    return new SwitchAst (switch_loc, NULL, statement, new ListAst (switch_loc));
  }

  Ast* Parser::SwitchBody ()
  {
    // ExprCaseClause = ExprSwitchCase ":" StatementList .
    // ExprSwitchCase = "case" ExpressionList | "default" .
    // TypeCaseClause  = TypeSwitchCase ":" StatementList .
    // TypeSwitchCase  = "case" TypeList | "default" .
    // TypeList        = Type { "," Type } .

    ListAst* body = new ListAst (peek ().location ());

    want (Token::LeftBrace);
    while (peek () != Token::End &&
           peek () != Token::RightBrace)
      {
        Location loc = peek ().location ();
        Ast* expressionList;
        if (got (Token::Default))
          {
            expressionList = NULL;
          }
        else
          {
            want (Token::Case);
            expressionList = ExpressionList ();
          }
        want (Token::Colon);
        body->append (new CaseAst (loc, expressionList, StatementList ()));
      }
    want (Token::RightBrace);

    return body;
  }
  
  Ast* Parser::ForStmt ()
  {
    // ForStmt = "for" [ Condition | ForClause | RangeClause ] Block .
    // Condition = Expression .

    // ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
    // InitStmt = SimpleStmt .
    // PostStmt = SimpleStmt .
    // RangeClause = [ ExpressionList "=" | IdentifierList ":=" ] "range" Expression .

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

    Location for_loc = peek ().location ();
    want (Token::For);

    if (peek () == Token::LeftBrace)
      {
        // No condition.
        Ast* forClause = new ForClauseAst (for_loc, NULL, NULL, NULL);
        return new ForAst (for_loc, forClause, Block ());
      }

    Ast* initStmt;
    Ast* condition;
    Ast* postStmt;

    int outer = m_expression_level;
    m_expression_level = -1;

    // InitStmt
    if (peek () == Token::Semicolon)
      {
        // Not there.
        initStmt = NULL;
      }
    else
      {
        initStmt = SimpleStmt ();

        if (peek () == Token::LeftBrace)
          {
            // Condition.
            Ast* forClause = new ForClauseAst (for_loc, NULL, initStmt, NULL);
            m_expression_level = outer;
            return new ForAst (for_loc, forClause, Block ());
          }
      }

    want (Token::Semicolon);
    // Condition
    if (peek () == Token::Semicolon)
      {
        // Not there.
        condition = NULL;
      }
    else
      {
        condition = Expression ();
      }
    want (Token::Semicolon);
    // PostStmt
    if (peek () == Token::LeftBrace)
      {
        // Not there.
        postStmt = NULL;
      }
    else
      {
        postStmt = SimpleStmt ();
      }
    Ast* forClause = new ForClauseAst (for_loc, initStmt, condition, postStmt);
    m_expression_level = outer;
    return new ForAst (for_loc, forClause, Block ());
  }

  Ast* Parser::SourceFile ()
  {
    // SourceFile       = PackageClause ";" { ImportDecl ";" } { TopLevelDecl ";" } .
    // PackageClause  = "package" PackageName .
    // PackageName    = identifier .

    Location package_loc = peek ().location ();
    want (Token::Package);
    Ast* package = Identifier ();
    want (Token::Semicolon);
    ImportDeclListAst* imports = new ImportDeclListAst (peek ().location ());
    while (peek () == Token::Import)
      {
        imports->append (ImportDecl ());
        want (Token::Semicolon);
      }
    ListAst* decls = new ListAst (peek ().location ());
    while (peek () != Token::End)
      {
        Ast* topLevelDecl = TopLevelDecl ();
        if (topLevelDecl != NULL)
          {
            decls->append (topLevelDecl);
            want (Token::Semicolon);
          }
        advance (Token::Const, Token::Type, Token::Var, Token::Func, Token::End);
      }
    want (Token::End);
    return new SourceFileAst (package_loc, package, imports, decls);
  }

  Ast* Parser::SourceFileHeader ()
  {
    // SourceFile       = PackageClause ";" { ImportDecl ";" } .
    // PackageClause  = "package" PackageName .
    // PackageName    = identifier .

    Location package_loc = peek ().location ();
    want (Token::Package);
    Ast* package = Identifier ();
    want (Token::Semicolon);
    ImportDeclListAst* imports = new ImportDeclListAst (peek ().location ());
    while (peek () == Token::Import)
      {
        imports->append (ImportDecl ());
        want (Token::Semicolon);
      }
    ListAst* decls = new ListAst (peek ().location ());
    return new SourceFileAst (package_loc, package, imports, decls);
  }

  Ast* Parser::ImportDecl ()
  {
    // ImportDecl       = "import" ( ImportSpec | "(" { ImportSpec ";" } ")" ) .

    ImportDeclAst* importDecl = new ImportDeclAst (peek ().location ());
    want (Token::Import);

    if (peek ().kind () == Token::Literal && peek ().value ().kind == Value::String)
      {
        importDecl->append (ImportSpec ());
        return importDecl;
      }

    switch (peek ().kind ())
      {
      case Token::Period:
      case Token::Identifier:
        {
          importDecl->append (ImportSpec ());
          return importDecl;
        }

      case Token::LeftParen:
        {
          next ();
          while (peek () != Token::End &&
                 peek () != Token::RightParen)
            {
              importDecl->append (ImportSpec ());
              if (!optional_semicolon (Token::RightParen))
                {
                  break;
                }
            }
          want (Token::RightParen);
          return importDecl;
        }

      default:
        m_error_reporter << ExpectedASpecificationOrGroup (next ().location ());
        return importDecl;
      }
  }

  Ast* Parser::ImportSpec ()
  {
    // ImportSpec       = [ "." | PackageName ] ImportPath .
    // ImportPath       = string_lit .

    Location loc = peek ().location ();
    bool dot = false;
    Ast* identifier = NULL;
    if (got (Token::Period))
      {
        dot = true;
      }
    else if (peek () == Token::Identifier)
      {
        identifier = Identifier ();
      }

    if (peek ().kind () == Token::Literal && peek ().value ().kind == Value::String)
      {
        std::string path = peek ().value ().String_value;
        next ();
        return new ImportSpecAst (loc, dot, identifier, path);
      }
    else
      {
        return new ImportSpecAst (loc, dot, identifier, "");
      }
  }

  void Parser::trace (bool t)
  {
    m_trace = t;
  }

  bool Parser::trace () const
  {
    return m_trace;
  }

  Token Parser::peek (size_t offset)
  {
    return m_scanner.peek (offset);
  }

  bool Parser::got (Token::Kind kind)
  {
    if (peek () == kind)
      {
        next ();
        return true;
      }
    return false;
  }

  Token Parser::next ()
  {
    if (m_trace)
      {
        m_out << "Consumed " << peek () << std::endl;
      }
    return m_scanner.consume ();
  }

  bool Parser::want (Token::Kind kind)
  {
    if (!got (kind))
      {
        m_error_reporter << Expected1 (peek ().location (), kind);
        next ();
        return false;
      }
    return true;
  }

  void Parser::advance (Token::Kind kind, ...)
  {
    assert (kind != Token::End);

    std::set<Token::Kind> kinds;
    kinds.insert (kind);

    va_list args;
    va_start (args, kind);
    do {
      kind = static_cast<Token::Kind> (va_arg (args,int));
      kinds.insert (kind);
    } while (kind != Token::End);
    va_end (args);

    assert (kinds.count (Token::End) != 0);

    while (kinds.count (peek ().kind ()) == 0)
      {
        next ();
      }
  }

  bool Parser::optional_semicolon (Token::Kind follow)
  {
    switch (peek ().kind ())
      {
      case Token::Semicolon:
        next ();
        return true;
      case Token::RightParen:
      case Token::RightBrace:
        return true;
      default:
        m_error_reporter << Expected1 (peek ().location (), follow);
        advance (follow, Token::End);
        return false;
      }
  }

}
