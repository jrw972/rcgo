#ifndef Activation_hpp
#define Activation_hpp

#include "types.hpp"

struct Activation
{
  Activation (ast_t& n)
    : mode (ACTIVATION_READ)
    , node (n)
  { }

  ActivationMode mode;
  ast_t& node;
};

#endif /* Activation_hpp */
