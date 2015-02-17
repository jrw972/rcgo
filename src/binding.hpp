#ifndef binding_h
#define binding_h

#include "types.hpp"
#include <vector>

struct binding_t
{
  binding_t ()
    : reaction (NULL)
  { }

  typedef std::vector<field_t*> OutputsType;
  OutputsType outputs;
  typedef std::vector<field_t*> InputsType;
  InputsType inputs;
  reaction_t *reaction;
};

#endif /* binding_h */
