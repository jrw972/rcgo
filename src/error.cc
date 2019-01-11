// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error.h"

#include <cassert>
#include <sstream>

#include "src/symbol.h"

namespace rcgo {

Error::operator std::string () const {
  std::stringstream ss;
  Print(ss);
  return ss.str();
}

void Perror::Print(std::ostream& out) const {
  char buf[256];
  char* m = strerror_r(err, buf, 256);
  out << prefix << ": " << m << std::endl;
}

void PackageMismatch::Print(std::ostream& out) const {
  out << location << ": error: expected package " << expected_package
      << " but found " << package << std::endl;
}

void RecursiveImport::Print(std::ostream& out) const {
  // The last import_path is the problem.
  out << "error: recursive import of \""
      << path << "\" detected.  Import chain to follow:"
      << std::endl;

  for (const auto& p : path_list) {
    out << p << ((p == path) ? " *" : "") << std::endl;
  }
  out << path << " *" << std::endl;
}

void NoFiles::Print(std::ostream& out) const {
  out << "error: "
      << package_source_directory_path << " contains no files" << std::endl;
}

void DuplicateSymbol::Print(std::ostream& out) const {
  out << symbol->location << ": error: " << symbol->identifier
      << " was previously declared at " << previous_symbol->location
      << std::endl;
}

void NotDeclared::Print(std::ostream& out) const {
  out << location << ": error: " << identifier << " was not declared"
      << std::endl;
}

void DoesNotReferToAPackage::Print(std::ostream& out) const {
  out << location << ": error: " << identifier << " does not refer to a package"
      << std::endl;
}

void ArrayLengthRequired::Print(std::ostream& out) const {
  out << location << ": error: an array length is required" << std::endl;
}

void ArrayLengthNotAnInt::Print(std::ostream& out) const {
  out << location << ": error: array length is not an int" << std::endl;
}

void ArrayLengthIsNegative::Print(std::ostream& out) const {
  out << location << ": error: array length is negative" << std::endl;
}

void DoesNotReferToAType::Print(std::ostream& out) const {
  out << location << ": error: " << identifier << " does not refer to a type"
      << std::endl;
}

void DoesNotReferToAnInterface::Print(std::ostream& out) const {
  out << location << ": error: " << identifier
      << " does not refer to an interface" << std::endl;
}

void KeyTypeNotComparable::Print(std::ostream& out) const {
  out << location << ": error: key type not comparable" << std::endl;
}

void SymbolIsDefinedRecursively::Print(std::ostream& out) const {
  out << symbol->location << ": error: " << symbol->identifier
      << " is defined recursively" << std::endl;
}

void DoesNotReferToAConstant::Print(std::ostream& out) const {
  out << location << ": error: " << identifier
      << " does not refer to a constant" << std::endl;
}

void IsNotExported::Print(std::ostream& out) const {
  out << location << ": error: " << identifier << " is not exported"
      << std::endl;
}

void ExpectedAType::Print(std::ostream& out) const {
  out << location << ": error: expected a type" << std::endl;
}

void ExpectedAFieldDeclaration::Print(std::ostream& out) const {
  out << location << ": error: expected a field declaration" << std::endl;
}

static const char* aOrAn(const std::string& str) {
  assert(!str.empty());

  switch (str[0]) {
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'i':
    case 'I':
    case 'o':
    case 'O':
    case 'u':
    case 'U':
      return "an";
    default:
      return "a";
  }
}

void Expected1::Print(std::ostream& out) const {
  std::stringstream expectedString;
  expectedString << expected;
  out << location << ": error: expected " << aOrAn(expectedString.str()) << ' '
      << expectedString.str() << std::endl;
}

void IllegalVariadicParameter::Print(std::ostream& out) const {
  out << location << ": error: illegal variadic parameter" << std::endl;
}

void ExpectedAMethodSpecification::Print(std::ostream& out) const {
  out << location << ": error: expected a method specification" << std::endl;
}

void ExpectedAnOperand::Print(std::ostream& out) const {
  out << location << ": error: expected an operand" << std::endl;
}

void ExpectedADeclaration::Print(std::ostream& out) const {
  out << location << ": error: expected a declaration" << std::endl;
}

void ExpectedASpecificationOrGroup::Print(std::ostream& out) const {
  out << location << ": error: expected a specification or group" << std::endl;
}

void ExpectedNExpressions::Print(std::ostream& out) const {
  out << location << ": error: expected " << n << " expressions" << std::endl;
}

void InvalidSlice::Print(std::ostream& out) const {
  out << location << ": error: invalid slice" << std::endl;
}

void UnexpectedExpressionList::Print(std::ostream& out) const {
  out << location << ": error: unexpected expression list" << std::endl;
}

void Expected2::Print(std::ostream& out) const {
  std::stringstream expectedString1, expectedString2;
  expectedString1 << expecteda;
  expectedString2 << expectedb;
  out << location << ": error: expected " << aOrAn(expectedString1.str()) << ' '
      << expectedString1.str() << " or " << aOrAn(expectedString2.str()) << ' '
      << expectedString2.str() << std::endl;
}

void IllegalUtf8Sequence::Print(std::ostream& out) const {
  out << location << ": error: illegal UTF-8 sequence" << std::endl;
}

void FloatingPointOverflow::Print(std::ostream& out) const {
  out << location << ": error: floating point overflow" << std::endl;
}

void ExpectedDecimalDigit::Print(std::ostream& out) const {
  out << location << ": error: expected decimal digit" << std::endl;
}

void IncompleteRuneLiteral::Print(std::ostream& out) const {
  out << location << ": error: incomplete rune literal" << std::endl;
}

void EmptyRuneLiteral::Print(std::ostream& out) const {
  out << location << ": error: empty rune literal" << std::endl;
}

void ExtraCharactersInRuneLiteral::Print(std::ostream& out) const {
  out << location << ": error: extra characters in rune literal" << std::endl;
}

void OctalValueOutOfRange::Print(std::ostream& out) const {
  out << location << ": error: octal value out of range" << std::endl;
}

void TooFewOctalDigitsInRuneLiteral::Print(std::ostream& out) const {
  out << location << ": error: too few octal digits in rune literal"
      << std::endl;
}

void TooFewHexadecimalDigitsInRuneLiteral::Print(std::ostream& out) const {
  out << location << ": error: too few hexadecimal digits in rune literal"
      << std::endl;
}

void InvalidUnicodeCodePoint::Print(std::ostream& out) const {
  out << location << ": error: invalid unicode code point" << std::endl;
}

void IllegalEscapeSequence::Print(std::ostream& out) const {
  out << location << ": error: illegal escape sequence" << std::endl;
}

void IncompleteStringLiteral::Print(std::ostream& out) const {
  out << location << ": error: incomplete string literal" << std::endl;
}

void ExtraCharactersInToken::Print(std::ostream& out) const {
  out << location << ": error: extra characters in token" << std::endl;
}

void StrayRune::Print(std::ostream& out) const {
  out << location << ": error: stray rune '" << rune << "'" << std::endl;
}

void UnterminatedGeneralComment::Print(std::ostream& out) const {
  out << location << ": error: unterminated comment" << std::endl;
}

void CannotApply::Print(std::ostream& out) const {
  out << location << ": error: cannot apply '" << operator_ << "' to "
      << value << std::endl;
}

void CannotApply2::Print(std::ostream& out) const {
  out << location << ": error: cannot apply '" << operator_ << "' to "
      << value1 << " and " << value2 << std::endl;
}

void DivisionByZero::Print(std::ostream& out) const {
  out << location << ": error: division by zero" << std::endl;
}

void OperandCannotBeCalled::Print(std::ostream& out) const {
  out << location << ": error: operand cannot be called" << std::endl;
}

void UnexpectedMultiValueOperand::Print(std::ostream& out) const {
  out << location << ": error: unexpected multi-value operand" << std::endl;
}

void CallExpectsNArguments::Print(std::ostream& out) const {
  out << location << ": error: call expects " << expected
      << " arguments but received " << actual << std::endl;
}

void CannotConvert::Print(std::ostream& out) const {
  out << location << ": error: cannot convert " << value
      << " to " << type << std::endl;
}

}  // namespace rcgo
