// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error.h"

#include <string>

#include "src/catch.hpp"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

TEST_CASE("Perror::Perror()") {
  Perror err("message", 2);
  REQUIRE(std::string(err) == "message: No such file or directory\n");
}

TEST_CASE("CouldNotFindPackage::CouldNotFindPackage()") {
  CouldNotFindPackage err(import_location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: Could not find package for path \"path\"\n");
}

TEST_CASE("PackageMismatch::PackageMismatch()") {
  PackageMismatch err(location, "expected", "actual");
  REQUIRE(std::string(err) ==
          "undefined:1: error: expected package expected but found actual\n");
}

TEST_CASE("RecursiveImport::RecursiveImport()") {
  ImportLocations import_locations;
  import_locations.push_back(import_location);
  RecursiveImport err(import_locations);
  REQUIRE(std::string(err) ==
          "undefined:1: error: recursive import of \"path\" detected.  "
          "Import chain to follow:\nundefined:1 imports \"path\" *\n");
}

TEST_CASE("NoFiles::NoFiles()") {
  NoFiles err(import_location, "path");
  REQUIRE(std::string(err) == "undefined:1: error: path contains no files\n");
}

TEST_CASE("DuplicateSymbol::DuplicateSymbol()") {
  DuplicateSymbol err(&test_symbol, &test_symbol);
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier was previously declared at"
          " undefined:1\n");
}

TEST_CASE("NotDeclared::NotDeclared()") {
  NotDeclared err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier was not declared\n");
}

TEST_CASE("DoesNotReferToAPackage::DoesNotReferToAPackage()") {
  DoesNotReferToAPackage err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier does not refer to a package\n");
}

TEST_CASE("ArrayLengthRequired::ArrayLengthRequired()") {
  ArrayLengthRequired err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: an array length is required\n");
}

TEST_CASE("ArrayLengthNotAnInt::ArrayLengthNotAnInt()") {
  ArrayLengthNotAnInt err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: array length is not an int\n");
}

TEST_CASE("ArrayLengthIsNegative::ArrayLengthIsNegative()") {
  ArrayLengthIsNegative err(location);
  REQUIRE(std::string(err) == "undefined:1: error: array length is negative\n");
}

TEST_CASE("DoesNotReferToAType::DoesNotReferToAType()") {
  DoesNotReferToAType err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier does not refer to a type\n");
}

TEST_CASE("DoesNotReferToAnInterface::DoesNotReferToAnInterface()") {
  DoesNotReferToAnInterface err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier does not refer to an interface\n");
}

TEST_CASE("KeyTypeNotComparable::KeyTypeNotComparable()") {
  KeyTypeNotComparable err(location);
  REQUIRE(std::string(err) == "undefined:1: error: key type not comparable\n");
}

TEST_CASE("SymbolIsDefinedRecursively::SymbolIsDefinedRecursively()") {
  SymbolIsDefinedRecursively err(&test_symbol);
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier is defined recursively\n");
}

TEST_CASE("DoesNotReferToAConstant::DoesNotReferToAConstant()") {
  DoesNotReferToAConstant err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier does not refer to a constant\n");
}

TEST_CASE("IsNotExported::IsNotExported()") {
  IsNotExported err(location, "identifier");
  REQUIRE(std::string(err) ==
          "undefined:1: error: identifier is not exported\n");
}

TEST_CASE("ExpectedAType::ExpectedAType()") {
  ExpectedAType err(location);
  REQUIRE(std::string(err) == "undefined:1: error: expected a type\n");
}

TEST_CASE("ExpectedAFieldDeclaration::ExpectedAFieldDeclaration()") {
  ExpectedAFieldDeclaration err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: expected a field declaration\n");
}

TEST_CASE("Expected1::Expected1()") {
  SECTION("a") {
    Expected1 err(location, Token::kFor);
    REQUIRE(std::string(err) == "undefined:1: error: expected a for\n");
  }
  SECTION("an") {
    Expected1 err(location, Token::kIf);
    REQUIRE(std::string(err) == "undefined:1: error: expected an if\n");
  }
}

TEST_CASE("IllegalVariadicParameter::IllegalVariadicParameter()") {
  IllegalVariadicParameter err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: illegal variadic parameter\n");
}

TEST_CASE("ExpectedAMethodSpecification::ExpectedAMethodSpecification()") {
  ExpectedAMethodSpecification err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: expected a method specification\n");
}

TEST_CASE("ExpectedAnOperand::ExpectedAnOperand()") {
  ExpectedAnOperand err(location);
  REQUIRE(std::string(err) == "undefined:1: error: expected an operand\n");
}

TEST_CASE("ExpectedADeclaration::ExpectedADeclaration()") {
  ExpectedADeclaration err(location);
  REQUIRE(std::string(err) == "undefined:1: error: expected a declaration\n");
}

TEST_CASE("ExpectedASpecificationOrGroup::ExpectedASpecificationOrGroup()") {
  ExpectedASpecificationOrGroup err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: expected a specification or group\n");
}

TEST_CASE("ExpectedNExpressions::ExpectedNExpressions()") {
  ExpectedNExpressions err(location, 5);
  REQUIRE(std::string(err) == "undefined:1: error: expected 5 expressions\n");
}

TEST_CASE("InvalidSlice::InvalidSlice()") {
  InvalidSlice err(location);
  REQUIRE(std::string(err) == "undefined:1: error: invalid slice\n");
}

TEST_CASE("UnexpectedExpressionList::UnexpectedExpressionList()") {
  UnexpectedExpressionList err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: unexpected expression list\n");
}

TEST_CASE("Expected2::Expected2()") {
  Expected2 err(location, Token::kFor, Token::kIf);
  REQUIRE(std::string(err) == "undefined:1: error: expected a for or an if\n");
}

TEST_CASE("IllegalUtf8Sequence::IllegalUtf8Sequence()") {
  IllegalUtf8Sequence err(location);
  REQUIRE(std::string(err) == "undefined:1: error: illegal UTF-8 sequence\n");
}

TEST_CASE("FloatingPointOverflow::FloatingPointOverflow()") {
  FloatingPointOverflow err(location);
  REQUIRE(std::string(err) == "undefined:1: error: floating point overflow\n");
}

TEST_CASE("ExpectedDecimalDigit::ExpectedDecimalDigit()") {
  ExpectedDecimalDigit err(location);
  REQUIRE(std::string(err) == "undefined:1: error: expected decimal digit\n");
}

TEST_CASE("IncompleteRuneLiteral::IncompleteRuneLiteral()") {
  IncompleteRuneLiteral err(location);
  REQUIRE(std::string(err) == "undefined:1: error: incomplete rune literal\n");
}

TEST_CASE("EmptyRuneLiteral::EmptyRuneLiteral()") {
  EmptyRuneLiteral err(location);
  REQUIRE(std::string(err) == "undefined:1: error: empty rune literal\n");
}

TEST_CASE("ExtraCharactersInRuneLiteral::ExtraCharactersInRuneLiteral()") {
  ExtraCharactersInRuneLiteral err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: extra characters in rune literal\n");
}

TEST_CASE("OctalValueOutOfRange::OctalValueOutOfRange()") {
  OctalValueOutOfRange err(location);
  REQUIRE(std::string(err) == "undefined:1: error: octal value out of range\n");
}

TEST_CASE("TooFewOctalDigitsInRuneLiteral::TooFewOctalDigitsInRuneLiteral()") {
  TooFewOctalDigitsInRuneLiteral err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: too few octal digits in rune literal\n");
}

TEST_CASE("TooFewHexadecimalDigitsInRuneLiteral::"
          "TooFewHexadecimalDigitsInRuneLiteral()") {
  TooFewHexadecimalDigitsInRuneLiteral err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: too few hexadecimal digits in rune literal\n");
}

TEST_CASE("InvalidUnicodeCodePoint::InvalidUnicodeCodePoint()") {
  InvalidUnicodeCodePoint err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: invalid unicode code point\n");
}

TEST_CASE("IllegalEscapeSequence::IllegalEscapeSequence()") {
  IllegalEscapeSequence err(location);
  REQUIRE(std::string(err) == "undefined:1: error: illegal escape sequence\n");
}

TEST_CASE("IncompleteStringLiteral::IncompleteStringLiteral()") {
  IncompleteStringLiteral err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: incomplete string literal\n");
}

TEST_CASE("ExtraCharactersInToken::ExtraCharactersInToken()") {
  ExtraCharactersInToken err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: extra characters in token\n");
}

TEST_CASE("StrayRune::StrayRune()") {
  StrayRune err(location, "x");
  REQUIRE(std::string(err) == "undefined:1: error: stray rune 'x'\n");
}

TEST_CASE("UnterminatedGeneralComment::UnterminatedGeneralComment()") {
  UnterminatedGeneralComment err(location);
  REQUIRE(std::string(err) ==
          "undefined:1: error: unterminated comment\n");
}

}  // namespace test
}  // namespace rcgo
