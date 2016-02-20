#include "reaction.hpp"

#include "ast.hpp"
#include "runtime.hpp"

namespace decl
{

void
Reaction::call (runtime::ExecutorBase& exec) const
{
  operation->execute (exec);
}

}
