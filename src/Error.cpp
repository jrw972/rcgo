/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Error.hpp"
#include "Symbol.hpp"
#include <sstream>

namespace rcgo {

  Error::~Error()
  { }

  Error::operator std::string () const
  {
    std::stringstream ss;
    print (ss);
    return ss.str ();
  }

  Perror::Perror (const std::string& a_prefix,
                  int a_err)
    : prefix (a_prefix)
    , err (a_err)
  { }

  void Perror::print (std::ostream& out) const
  {
    char buf[256];
    char* m = strerror_r (err, buf, 256);
    out << prefix << ": " << m << std::endl;
  }

  CouldNotFindPackage::CouldNotFindPackage (const ImportLocation& a_import_location)
    : import_location (a_import_location)
  { }

  void CouldNotFindPackage::print (std::ostream& out) const
  {
    out << import_location.location << ": error: Could not find package for path \"" << import_location.import_path << "\"" << std::endl;
  }

  PackageMismatch::PackageMismatch (const Location& a_location,
                                    const std::string& a_expected_package,
                                    const std::string& a_package)
    : location (a_location)
    , expected_package (a_expected_package)
    , package (a_package)
  { }

  void PackageMismatch::print (std::ostream& out) const
  {
    out << location << ": error: expected package " << expected_package << " but found " << package << std::endl;
  }

  RecursiveImport::RecursiveImport (const ImportLocations& a_import_locations)
    : import_locations (a_import_locations)
  { }

  void RecursiveImport::print (std::ostream& out) const
  {
    // The last import_path is the problem.
    const std::string& problem_import_path = import_locations.back ().import_path;
    out << import_locations.back ().location << ": error: recursive import of \"" << problem_import_path << "\" detected.  Import chain to follow:" << std::endl;

    for (ImportLocations::const_iterator pos = import_locations.begin (),
           limit = import_locations.end ();
         pos != limit;
         ++pos)
      {
        char m = (pos->import_path == problem_import_path) ? '*' : ' ';
        out << pos->location << " imports \"" << pos->import_path << "\" " << m << std::endl;
      }
  }

  NoFiles::NoFiles (const ImportLocation& a_import_location,
                    const std::string& a_package_source_directory_path)
    : import_location (a_import_location)
    , package_source_directory_path (a_package_source_directory_path)
  { }

  void NoFiles::print (std::ostream& out) const
  {
    out << import_location.location << ": error: " << package_source_directory_path << " contains no files" << std::endl;
  }

  DuplicateSymbol::DuplicateSymbol (const Symbol* a_symbol,
                                    const Symbol* a_previous_symbol)
    : symbol (a_symbol)
    , previous_symbol (a_previous_symbol)
  { }

  void DuplicateSymbol::print (std::ostream& out) const
  {
    out << symbol->location << ": error: " << symbol->identifier << " was previously declared at " << previous_symbol->location << std::endl;
  }

  NotDeclared::NotDeclared (const Location& a_location,
                            const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void NotDeclared::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " was not declared" << std::endl;
  }

  DoesNotReferToAPackage::DoesNotReferToAPackage (const Location& a_location,
                                                  const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void DoesNotReferToAPackage::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " does not refer to a package" << std::endl;
  }

  ArrayLengthRequired::ArrayLengthRequired (const Location& a_location)
    : location (a_location)
  { }

  void ArrayLengthRequired::print (std::ostream& out) const
  {
    out << location << ": error: an array length is required" << std::endl;
  }

  ArrayLengthNotAnInt::ArrayLengthNotAnInt (const Location& a_location)
    : location (a_location)
  { }

  void ArrayLengthNotAnInt::print (std::ostream& out) const
  {
    out << location << ": error: array length is not an int" << std::endl;
  }

  ArrayLengthIsNegative::ArrayLengthIsNegative (const Location& a_location)
    : location (a_location)
  { }

  void ArrayLengthIsNegative::print (std::ostream& out) const
  {
    out << location << ": error: array length is negative" << std::endl;
  }

  DoesNotReferToAType::DoesNotReferToAType (const Location& a_location,
                                            const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void DoesNotReferToAType::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " does not refer to a type" << std::endl;
  }

  DoesNotReferToAnInterface::DoesNotReferToAnInterface (const Location& a_location,
                                                        const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void DoesNotReferToAnInterface::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " does not refer to an interface" << std::endl;
  }

  KeyTypeNotComparable::KeyTypeNotComparable (const Location& a_location)
    : location (a_location)
  { }

  void KeyTypeNotComparable::print (std::ostream& out) const
  {
    out << location << ": error: key type not comparable" << std::endl;
  }

  SymbolIsDefinedRecursively::SymbolIsDefinedRecursively (const Symbol* a_symbol)
    : symbol (a_symbol)
  { }

  void SymbolIsDefinedRecursively::print (std::ostream& out) const
  {
    out << symbol->location << ": error: " << symbol->identifier << " is defined recursively" << std::endl;
  }

  DoesNotReferToAConstant::DoesNotReferToAConstant (const Location& a_location,
                                                    const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void DoesNotReferToAConstant::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " does not refer to a constant" << std::endl;
  }

  IsNotExported::IsNotExported (const Location& a_location,
                                const std::string& a_identifier)
    : location (a_location)
    , identifier (a_identifier)
  { }

  void IsNotExported::print (std::ostream& out) const
  {
    out << location << ": error: " << identifier << " is not exported" << std::endl;
  }

  ExpectedAType::ExpectedAType (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedAType::print (std::ostream& out) const
  {
    out << location << ": error: expected a type" << std::endl;
  }

  ExpectedAFieldDeclaration::ExpectedAFieldDeclaration (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedAFieldDeclaration::print (std::ostream& out) const
  {
    out << location << ": error: expected a field declaration" << std::endl;
  }

  static const char* aOrAn (const std::string& str)
  {
    assert (!str.empty ());

    switch (str[0])
      {
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

  Expected1::Expected1 (const Location& a_location,
                        const Token::Kind a_expected)
    : location (a_location)
    , expected (a_expected)
  { }

  void Expected1::print (std::ostream& out) const
  {
    std::stringstream expectedString;
    expectedString << expected;
    out << location << ": error: expected " << aOrAn (expectedString.str ()) << ' ' << expectedString.str () << std::endl;
  }

  IllegalVariadicParameter::IllegalVariadicParameter (const Location& a_location)
    : location (a_location)
  { }

  void IllegalVariadicParameter::print (std::ostream& out) const
  {
    out << location << ": error: illegal variadic parameter" << std::endl;
  }

  ExpectedAMethodSpecification::ExpectedAMethodSpecification (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedAMethodSpecification::print (std::ostream& out) const
  {
    out << location << ": error: expected a method specification" << std::endl;
  }

  ExpectedAnOperand::ExpectedAnOperand (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedAnOperand::print (std::ostream& out) const
  {
    out << location << ": error: expected an operand" << std::endl;
  }

  ExpectedADeclaration::ExpectedADeclaration (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedADeclaration::print (std::ostream& out) const
  {
    out << location << ": error: expected a declaration" << std::endl;
  }

  ExpectedASpecificationOrGroup::ExpectedASpecificationOrGroup (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedASpecificationOrGroup::print (std::ostream& out) const
  {
    out << location << ": error: expected a specification or group" << std::endl;
  }

  ExpectedNExpressions::ExpectedNExpressions (const Location& a_location,
                                              size_t a_n)
    : location (a_location)
    , n (a_n)
  { }

  void ExpectedNExpressions::print (std::ostream& out) const
  {
    out << location << ": error: expected " << n << " expressions" << std::endl;
  }

  InvalidSlice::InvalidSlice (const Location& a_location)
    : location (a_location)
  { }

  void InvalidSlice::print (std::ostream& out) const
  {
    out << location << ": error: invalid slice" << std::endl;
  }

  UnexpectedExpressionList::UnexpectedExpressionList (const Location& a_location)
    : location (a_location)
  { }

  void UnexpectedExpressionList::print (std::ostream& out) const
  {
    out << location << ": error: unexpected expression list" << std::endl;
  }

  Expected2::Expected2 (const Location& a_location,
                        const Token::Kind a_expecteda,
                        const Token::Kind a_expectedb)
    : location (a_location)
    , expecteda (a_expecteda)
    , expectedb (a_expectedb)
  { }

  void Expected2::print (std::ostream& out) const
  {
    std::stringstream expectedString1, expectedString2;
    expectedString1 << expecteda;
    expectedString2 << expectedb;
    out << location << ": error: expected " << aOrAn (expectedString1.str ()) << ' ' << expectedString1.str () << " or " << aOrAn (expectedString2.str ()) << ' ' << expectedString2.str () << std::endl;
  }

  IllegalUtf8Sequence::IllegalUtf8Sequence (const Location& a_location)
    : location (a_location)
  { }

  void IllegalUtf8Sequence::print (std::ostream& out) const
  {
    out << location << ": error: illegal UTF-8 sequence" << std::endl;
  }

  FloatingPointOverflow::FloatingPointOverflow (const Location& a_location)
    : location (a_location)
  { }

  void FloatingPointOverflow::print (std::ostream& out) const
  {
    out << location << ": error: floating point overflow" << std::endl;
  }

  ExpectedDecimalDigit::ExpectedDecimalDigit (const Location& a_location)
    : location (a_location)
  { }

  void ExpectedDecimalDigit::print (std::ostream& out) const
  {
    out << location << ": error: expected decimal digit" << std::endl;
  }

  IncompleteRuneLiteral::IncompleteRuneLiteral (const Location& a_location)
    : location (a_location)
  { }

  void IncompleteRuneLiteral::print (std::ostream& out) const
  {
    out << location << ": error: incomplete rune literal" << std::endl;
  }

  EmptyRuneLiteral::EmptyRuneLiteral (const Location& a_location)
    : location (a_location)
  { }

  void EmptyRuneLiteral::print (std::ostream& out) const
  {
    out << location << ": error: empty rune literal" << std::endl;
  }

  ExtraCharactersInRuneLiteral::ExtraCharactersInRuneLiteral (const Location& a_location)
    : location (a_location)
  { }

  void ExtraCharactersInRuneLiteral::print (std::ostream& out) const
  {
    out << location << ": error: extra characters in rune literal" << std::endl;
  }

  OctalValueOutOfRange::OctalValueOutOfRange (const Location& a_location)
    : location (a_location)
  { }

  void OctalValueOutOfRange::print (std::ostream& out) const
  {
    out << location << ": error: octal value out of range" << std::endl;
  }

  TooFewOctalDigitsInRuneLiteral::TooFewOctalDigitsInRuneLiteral (const Location& a_location)
    : location (a_location)
  { }

  void TooFewOctalDigitsInRuneLiteral::print (std::ostream& out) const
  {
    out << location << ": error: too few octal digits in rune literal" << std::endl;
  }

  TooFewHexadecimalDigitsInRuneLiteral::TooFewHexadecimalDigitsInRuneLiteral (const Location& a_location)
    : location (a_location)
  { }

  void TooFewHexadecimalDigitsInRuneLiteral::print (std::ostream& out) const
  {
    out << location << ": error: too few hexadecimal digits in rune literal" << std::endl;
  }

  InvalidUnicodeCodePoint::InvalidUnicodeCodePoint (const Location& a_location)
    : location (a_location)
  { }

  void InvalidUnicodeCodePoint::print (std::ostream& out) const
  {
    out << location << ": error: invalid unicode code point" << std::endl;
  }

  IllegalEscapeSequence::IllegalEscapeSequence (const Location& a_location)
    : location (a_location)
  { }

  void IllegalEscapeSequence::print (std::ostream& out) const
  {
    out << location << ": error: illegal escape sequence" << std::endl;
  }

  IncompleteStringLiteral::IncompleteStringLiteral (const Location& a_location)
    : location (a_location)
  { }

  void IncompleteStringLiteral::print (std::ostream& out) const
  {
    out << location << ": error: incomplete string literal" << std::endl;
  }

  ExtraCharactersInToken::ExtraCharactersInToken (const Location& a_location)
    : location (a_location)
  { }

  void ExtraCharactersInToken::print (std::ostream& out) const
  {
    out << location << ": error: extra characters in token" << std::endl;
  }

  StrayRune::StrayRune (const Location& a_location,
                        const std::string& a_rune)
    : location (a_location)
    , rune (a_rune)
  { }

  void StrayRune::print (std::ostream& out) const
  {
    out << location << ": error: stray rune '" << rune << "'" << std::endl;
  }

  UnterminatedGeneralComment::UnterminatedGeneralComment (const Location& a_location)
    : location (a_location)
  { }

  void UnterminatedGeneralComment::print (std::ostream& out) const
  {
    out << location << ": error: unterminated comment" << std::endl;
  }

}
