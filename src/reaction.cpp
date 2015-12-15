#include "reaction.hpp"

#include "ast.hpp"
#include "runtime.hpp"

namespace decl
{

void
reaction_t::call (runtime::executor_base_t& exec) const
{
  node->operation->execute (exec);
}

}
