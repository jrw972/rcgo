#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <string>

#include "src/import_location.h"
#include "src/symbol.h"
#include "src/token.h"

namespace rcgo {

struct Error {  // NOT_COVERED
  virtual ~Error() {}
  virtual void Print(std::ostream& out) const = 0;
  operator std::string() const;
};

struct Perror : public Error {  // NOT_COVERED
  Perror(const std::string& a_prefix, int a_err)
      : prefix(a_prefix), err(a_err) {}
  void Print(std::ostream& out) const override;

  std::string const prefix;
  int const err;
};

struct PackageMismatch : public Error {  // NOT_COVERED
  PackageMismatch(
      const Location& a_location, const std::string& a_expected_package,
      const std::string& a_package)
      : location(a_location), expected_package(a_expected_package),
        package(a_package) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const expected_package;
  std::string const package;
};

struct RecursiveImport : public Error {  // NOT_COVERED
  explicit RecursiveImport(const Paths& a_path_list, const Path& a_path)
      : path_list(a_path_list), path(a_path) {}
  void Print(std::ostream& out) const override;

  Paths const path_list;
  Path const path;
};

struct NoFiles : public Error {  // NOT_COVERED
  explicit NoFiles(const std::string& a_package_source_directory_path)
      : package_source_directory_path(a_package_source_directory_path) {}
  void Print(std::ostream& out) const override;

  std::string const package_source_directory_path;
};

struct DuplicateSymbol : public Error {  // NOT_COVERED
  DuplicateSymbol(
      const symbol::Symbol* a_symbol, const symbol::Symbol* a_previous_symbol)
      : symbol(a_symbol), previous_symbol(a_previous_symbol) {}
  void Print(std::ostream& out) const override;

  const symbol::Symbol* const symbol;
  const symbol::Symbol* const previous_symbol;
};

struct NotDeclared : public Error {  // NOT_COVERED
  NotDeclared(const Location& a_location, const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct DoesNotReferToAPackage : public Error {  // NOT_COVERED
  DoesNotReferToAPackage(const Location& a_location,
                         const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct ArrayLengthRequired : public Error {  // NOT_COVERED
  explicit ArrayLengthRequired(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ArrayLengthNotAnInt : public Error {  // NOT_COVERED
  explicit ArrayLengthNotAnInt(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ArrayLengthIsNegative : public Error {  // NOT_COVERED
  explicit ArrayLengthIsNegative(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct DoesNotReferToAType : public Error {  // NOT_COVERED
  DoesNotReferToAType(const Location& a_location,
                      const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct DoesNotReferToAnInterface : public Error {  // NOT_COVERED
  DoesNotReferToAnInterface(const Location& a_location,
                            const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct KeyTypeNotComparable : public Error {  // NOT_COVERED
  explicit KeyTypeNotComparable(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct SymbolIsDefinedRecursively : public Error {  // NOT_COVERED
  explicit SymbolIsDefinedRecursively(const symbol::Symbol* a_symbol)
      : symbol(a_symbol) {}
  void Print(std::ostream& out) const override;

  const symbol::Symbol* const symbol;
};

struct DoesNotReferToAConstant : public Error {  // NOT_COVERED
  DoesNotReferToAConstant(const Location& a_location,
                          const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct IsNotExported : public Error {  // NOT_COVERED
  IsNotExported(const Location& a_location, const std::string& a_identifier)
      : location(a_location), identifier(a_identifier) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const identifier;
};

struct ExpectedAType : public Error {  // NOT_COVERED
  explicit ExpectedAType(const Location& a_location): location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedAFieldDeclaration : public Error {  // NOT_COVERED
  explicit ExpectedAFieldDeclaration(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct Expected1 : public Error {  // NOT_COVERED
  Expected1(const Location& a_location, const Token::Kind a_expected)
      : location(a_location), expected(a_expected) {}
  void Print(std::ostream& out) const override;

  Location const location;
  Token::Kind const expected;
};

struct IllegalVariadicParameter : public Error {  // NOT_COVERED
  explicit IllegalVariadicParameter(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedAMethodSpecification : public Error {  // NOT_COVERED
  explicit ExpectedAMethodSpecification(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedAnOperand : public Error {  // NOT_COVERED
  explicit ExpectedAnOperand(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedADeclaration : public Error {  // NOT_COVERED
  explicit ExpectedADeclaration(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedASpecificationOrGroup : public Error {  // NOT_COVERED
  explicit ExpectedASpecificationOrGroup(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedNExpressions : public Error {  // NOT_COVERED
  ExpectedNExpressions(const Location& a_location, size_t a_n)
      : location(a_location), n(a_n) {}
  void Print(std::ostream& out) const override;

  Location const location;
  int const n;
};

struct InvalidSlice : public Error {  // NOT_COVERED
  explicit InvalidSlice(const Location& a_location)
      : location(a_location)  {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct UnexpectedExpressionList : public Error {  // NOT_COVERED
  explicit UnexpectedExpressionList(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct Expected2 : public Error {  // NOT_COVERED
  Expected2(const Location& a_location, const Token::Kind a_expecteda,
            const Token::Kind a_expectedb)
      : location(a_location), expecteda(a_expecteda), expectedb(a_expectedb) {}
  void Print(std::ostream& out) const override;

  Location const location;
  Token::Kind const expecteda;
  Token::Kind const expectedb;
};

struct IllegalUtf8Sequence : public Error {  // NOT_COVERED
  explicit IllegalUtf8Sequence(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct FloatingPointOverflow : public Error {  // NOT_COVERED
  explicit FloatingPointOverflow(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExpectedDecimalDigit : public Error {  // NOT_COVERED
  explicit ExpectedDecimalDigit(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct IncompleteRuneLiteral : public Error {  // NOT_COVERED
  explicit IncompleteRuneLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct EmptyRuneLiteral : public Error {  // NOT_COVERED
  explicit EmptyRuneLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExtraCharactersInRuneLiteral : public Error {  // NOT_COVERED
  explicit ExtraCharactersInRuneLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct OctalValueOutOfRange : public Error {  // NOT_COVERED
  explicit OctalValueOutOfRange(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct TooFewOctalDigitsInRuneLiteral : public Error {  // NOT_COVERED
  explicit TooFewOctalDigitsInRuneLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct TooFewHexadecimalDigitsInRuneLiteral : public Error {  // NOT_COVERED
  explicit TooFewHexadecimalDigitsInRuneLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct InvalidUnicodeCodePoint : public Error {  // NOT_COVERED
  explicit InvalidUnicodeCodePoint(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct IllegalEscapeSequence : public Error {  // NOT_COVERED
  explicit IllegalEscapeSequence(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct IncompleteStringLiteral : public Error {  // NOT_COVERED
  explicit IncompleteStringLiteral(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct ExtraCharactersInToken : public Error {  // NOT_COVERED
  explicit ExtraCharactersInToken(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct StrayRune : public Error {  // NOT_COVERED
  StrayRune(const Location& a_location, const std::string& a_rune)
      : location(a_location) , rune(a_rune) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const rune;
};

struct UnterminatedGeneralComment : public Error {  // NOT_COVERED
  explicit UnterminatedGeneralComment(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct CannotApply : public Error {  // NOT_COVERED
  CannotApply(const Location& a_location, char a_operator,
              value::Value const & a_value)
      : location(a_location), operator_(a_operator), value(a_value) {}
  void Print(std::ostream& out) const override;

  Location const location;
  char const operator_;
  value::Value const value;
};

struct CannotApply2 : public Error {  // NOT_COVERED
  CannotApply2(const Location& a_location, const std::string& a_operator,
               value::Value const & a_value1,
               value::Value const & a_value2)
      : location(a_location), operator_(a_operator), value1(a_value1),
        value2(a_value2) {}
  void Print(std::ostream& out) const override;

  Location const location;
  std::string const operator_;
  value::Value const value1;
  value::Value const value2;
};

struct DivisionByZero : public Error {  // NOT_COVERED
  explicit DivisionByZero(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct OperandCannotBeCalled : public Error {  // NOT_COVERED
  explicit OperandCannotBeCalled(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct UnexpectedMultiValueOperand : public Error {  // NOT_COVERED
  explicit UnexpectedMultiValueOperand(const Location& a_location)
      : location(a_location) {}
  void Print(std::ostream& out) const override;

  Location const location;
};

struct CallExpectsNArguments : public Error {  // NOT_COVERED
  CallExpectsNArguments(
      const Location& a_location, size_t a_expected, size_t a_actual)
      : location(a_location), expected(a_expected), actual(a_actual) {}
  void Print(std::ostream& out) const override;

  Location const location;
  size_t const expected;
  size_t const actual;
};

struct CannotConvert : public Error {  // NOT_COVERED
  CannotConvert(
      const Location& a_location, value::Value const & a_value,
      const type::Type* a_type)
      : location(a_location), value(a_value), type(a_type) {}
  void Print(std::ostream& out) const override;

  Location const location;
  value::Value const value;
  const type::Type* const type;
};

}  // namespace rcgo

#endif  // SRC_ERROR_H_
