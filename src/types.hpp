#ifndef RCGO_SRC_TYPES_HPP
#define RCGO_SRC_TYPES_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Forward declarations.

#include "config.hpp"

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <dirent.h>

#define PRECISION 256

namespace rcgo {
  struct AliasType;
  struct AndAst;
  struct AnonymousMethodSpecAst;
  struct ArrayAst;
  struct ArrayType;
  struct AssignAst;
  struct Ast;
  struct AstVisitor;
  struct BinaryAst;
  struct Block;
  struct BoolType;
  struct BreakAst;
  struct ByteStream;
  struct CallAst;
  struct CaseAst;
  struct Complex128Type;
  struct Complex64Type;
  struct CompositeLiteralAst;
  struct ConstSpecAst;
  struct ContinueAst;
  struct DecrementAst;
  struct DefaultAstVisitor;
  struct DeferAst;
  struct DefinedType;
  struct EmbeddedFieldAst;
  struct EmptyStatementAst;
  struct ErrorAst;
  struct Error;
  struct ErrorReporter;
  struct ExpressionStatementAst;
  struct FallthroughAst;
  struct FieldAst;
  struct FieldSymbol;
  struct Float32Type;
  struct Float64Type;
  struct ForAst;
  struct ForClauseAst;
  struct FuncDeclAst;
  struct FunctionLiteralAst;
  struct FunctionType;
  struct FunctionTypeAst;
  struct GotoAst;
  struct IdentifierAst;
  struct IfAst;
  struct ImportDeclAst;
  struct ImportDeclListAst;
  struct ImportLocation;
  struct ImportSpecAst;
  struct IncrementAst;
  struct IndexAst;
  struct Int16Type;
  struct Int32Type;
  struct Int64Type;
  struct Int8Type;
  struct IntType;
  struct InterfaceAst;
  struct InterfaceMethodSymbol;
  struct InterfaceType;
  struct KeyedElementAst;
  struct LabeledStatementAst;
  struct ListAst;
  struct LiteralAst;
  struct Location;
  struct MapAst;
  struct MapType;
  struct MethodDeclAst;
  struct MethodSpecAst;
  struct NamedType;
  struct OrAst;
  struct Package;
  struct PackageCache;
  struct ParameterDeclAst;
  struct ParameterSymbol;
  struct PointerAst;
  struct PointerType;
  struct RangeAst;
  struct ReturnAst;
  struct Scanner;
  struct SelectorAst;
  struct SignatureAst;
  struct SliceAst;
  struct SliceType;
  struct SliceTypeAst;
  struct SourceFileAst;
  struct StringType;
  struct StructAst;
  struct StructType;
  struct SwitchAst;
  struct Symbol;
  struct SymbolTable;
  struct SymbolVisitor;
  struct Token;
  struct Type;
  struct TypeAssertionAst;
  struct TypeExpressionAst;
  struct TypeFactory;
  struct TypeSpecAst;
  struct TypeVisitor;
  struct Uint16Type;
  struct Uint32Type;
  struct Uint64Type;
  struct Uint8Type;
  struct UintType;
  struct UintptrType;
  struct UnaryAst;
  struct VarSpecAst;

  typedef std::vector<std::string> Paths;
  typedef std::vector<Ast*> SourceFiles;
}

#endif // RCGO_SRC_TYPES_HPP
