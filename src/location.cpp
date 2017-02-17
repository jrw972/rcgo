#include "location.hpp"
#include "source_file.hpp"

namespace source
{

  Location::Location (const SourceFile* a_file, unsigned int a_line)
  : file_ (a_file)
  , line_ (a_line)
{ }

bool Location::operator== (const Location& other) const
{
  return this->file_ == other.file_ && this->line_ == other.line_;
}

  const Location Location::builtin (&SourceFile::builtin, 1);
}
