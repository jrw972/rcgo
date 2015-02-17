#ifndef trigger_hpp
#define trigger_hpp

#include "types.hpp"

struct trigger_t
{
  trigger_t (ast_t& n)
    : action (TRIGGER_READ)
    , node (n)
  { }

  TriggerAction action;
  ast_t& node;
};

#endif /* trigger_hpp */
