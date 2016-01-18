#include "reaction.hpp"

#include "ast.hpp"
#include "runtime.hpp"

namespace decl
{

void
Reaction::call (runtime::executor_base_t& exec) const
{
  operation->execute (exec);
}

}
