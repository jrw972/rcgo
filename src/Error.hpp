#ifndef RCGO_SRC_ERROR_HPP
#define RCGO_SRC_ERROR_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"
#include "ImportLocation.hpp"
#include "Token.hpp"

namespace rcgo {

  struct Error { // NOT_COVERED
    virtual ~Error ();
    virtual void print (std::ostream& out) const = 0;
    operator std::string () const;
  };

  struct Perror : public Error { // NOT_COVERED
    Perror (const std::string& a_prefix,
            int a_err);
    void print (std::ostream& out) const;

    std::string const prefix;
    int const err;
  };

  struct CouldNotFindPackage : public Error { // NOT_COVERED
    CouldNotFindPackage (const ImportLocation& a_import_location);
    void print (std::ostream& out) const;

    ImportLocation const import_location;
  };

  struct PackageMismatch : public Error { // NOT_COVERED
    PackageMismatch (const Location& a_location,
                     const std::string& a_expected_package,
                     const std::string& a_package);
    void print (std::ostream& out) const;

    Location const location;
    std::string const expected_package;
    std::string const package;
  };

  struct RecursiveImport : public Error { // NOT_COVERED
    RecursiveImport (const ImportLocations& a_import_locations);
    void print (std::ostream& out) const;

    ImportLocations const import_locations;
  };

  struct NoFiles : public Error { // NOT_COVERED
    NoFiles (const ImportLocation& a_import_location,
             const std::string& a_package_source_directory_path);
    void print (std::ostream& out) const;

    ImportLocation const import_location;
    std::string const package_source_directory_path;
  };

  struct DuplicateSymbol : public Error { // NOT_COVERED
    DuplicateSymbol (const Symbol* a_symbol,
                     const Symbol* a_previous_symbol);
    void print (std::ostream& out) const;

    const Symbol* const symbol;
    const Symbol* const previous_symbol;
  };

  struct NotDeclared : public Error { // NOT_COVERED
    NotDeclared (const Location& a_location,
                 const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct DoesNotReferToAPackage : public Error { // NOT_COVERED
    DoesNotReferToAPackage (const Location& a_location,
                            const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct ArrayLengthRequired : public Error { // NOT_COVERED
    ArrayLengthRequired (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ArrayLengthNotAnInt : public Error { // NOT_COVERED
    ArrayLengthNotAnInt (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ArrayLengthIsNegative : public Error { // NOT_COVERED
    ArrayLengthIsNegative (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct DoesNotReferToAType : public Error { // NOT_COVERED
    DoesNotReferToAType (const Location& a_location,
                         const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct DoesNotReferToAnInterface : public Error { // NOT_COVERED
    DoesNotReferToAnInterface (const Location& a_location,
                               const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct KeyTypeNotComparable : public Error { // NOT_COVERED
    KeyTypeNotComparable (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct SymbolIsDefinedRecursively : public Error { // NOT_COVERED
    SymbolIsDefinedRecursively (const Symbol* a_symbol);
    void print (std::ostream& out) const;

    const Symbol* const symbol;
  };

  struct DoesNotReferToAConstant : public Error { // NOT_COVERED
    DoesNotReferToAConstant (const Location& a_location,
                             const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct IsNotExported : public Error { // NOT_COVERED
    IsNotExported (const Location& a_location,
                   const std::string& a_identifier);
    void print (std::ostream& out) const;

    Location const location;
    std::string const identifier;
  };

  struct ExpectedAType : public Error { // NOT_COVERED
    ExpectedAType (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedAFieldDeclaration : public Error { // NOT_COVERED
    ExpectedAFieldDeclaration (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct Expected1 : public Error { // NOT_COVERED
    Expected1 (const Location& a_location,
               const Token::Kind a_expected);
    void print (std::ostream& out) const;

    Location const location;
    Token::Kind const expected;
  };

  struct IllegalVariadicParameter : public Error { // NOT_COVERED
    IllegalVariadicParameter (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedAMethodSpecification : public Error { // NOT_COVERED
    ExpectedAMethodSpecification (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedAnOperand : public Error { // NOT_COVERED
    ExpectedAnOperand (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedADeclaration : public Error { // NOT_COVERED
    ExpectedADeclaration (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedASpecificationOrGroup : public Error { // NOT_COVERED
    ExpectedASpecificationOrGroup (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedNExpressions : public Error { // NOT_COVERED
    ExpectedNExpressions (const Location& a_location,
                          size_t a_n);
    void print (std::ostream& out) const;

    Location const location;
    int const n;
  };

  struct InvalidSlice : public Error { // NOT_COVERED
    InvalidSlice (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct UnexpectedExpressionList : public Error { // NOT_COVERED
    UnexpectedExpressionList (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct Expected2 : public Error { // NOT_COVERED
    Expected2 (const Location& a_location,
               const Token::Kind a_expecteda,
               const Token::Kind a_expectedb);
    void print (std::ostream& out) const;

    Location const location;
    Token::Kind const expecteda;
    Token::Kind const expectedb;
  };

  struct IllegalUtf8Sequence : public Error { // NOT_COVERED
    IllegalUtf8Sequence (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct FloatingPointOverflow : public Error { // NOT_COVERED
    FloatingPointOverflow (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExpectedDecimalDigit : public Error { // NOT_COVERED
    ExpectedDecimalDigit (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct IncompleteRuneLiteral : public Error { // NOT_COVERED
    IncompleteRuneLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct EmptyRuneLiteral : public Error { // NOT_COVERED
    EmptyRuneLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExtraCharactersInRuneLiteral : public Error { // NOT_COVERED
    ExtraCharactersInRuneLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct OctalValueOutOfRange : public Error { // NOT_COVERED
    OctalValueOutOfRange (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct TooFewOctalDigitsInRuneLiteral : public Error { // NOT_COVERED
    TooFewOctalDigitsInRuneLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct TooFewHexadecimalDigitsInRuneLiteral : public Error { // NOT_COVERED
    TooFewHexadecimalDigitsInRuneLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct InvalidUnicodeCodePoint : public Error { // NOT_COVERED
    InvalidUnicodeCodePoint (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct IllegalEscapeSequence : public Error { // NOT_COVERED
    IllegalEscapeSequence (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct IncompleteStringLiteral : public Error { // NOT_COVERED
    IncompleteStringLiteral (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct ExtraCharactersInToken : public Error { // NOT_COVERED
    ExtraCharactersInToken (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

  struct StrayRune : public Error { // NOT_COVERED
    StrayRune (const Location& a_location,
               const std::string& a_rune);
    void print (std::ostream& out) const;

    Location const location;
    std::string const rune;
  };

  struct UnterminatedGeneralComment : public Error { // NOT_COVERED
    UnterminatedGeneralComment (const Location& a_location);
    void print (std::ostream& out) const;

    Location const location;
  };

}

#endif // RCGO_SRC_ERROR_HPP
