#include "reaction.hpp"

#include "ast.hpp"
#include "runtime.hpp"

void
reaction_t::call (executor_base_t& exec) const
{
  node->operation->execute (exec);
}
