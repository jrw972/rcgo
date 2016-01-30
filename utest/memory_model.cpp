#include "type.hpp"

#include "tap.hpp"
#include "memory_model.hpp"

using namespace runtime;

int
main (int argc, char** argv)
{
  Tap tap;

  arch::set_stack_alignment (sizeof (void*));

  {
    MemoryModel mm;
    tap.tassert ("MemoryModel::MemoryModel", mm.arguments_empty () && mm.locals_empty ());
  }

 {
    MemoryModel mm;
    ptrdiff_t ao = mm.arguments_offset ();
    mm.arguments_push (32);
    tap.tassert ("MemoryModel::arguments_push", mm.arguments_offset () == ao - 32);
    mm.set_receiver_offset ();
    tap.tassert ("MemoryModel::arguments_push", mm.receiver_offset () == ao - 32);
  }

 {
    MemoryModel mm;
    ptrdiff_t lo = mm.locals_offset ();
    mm.locals_push (32);
    tap.tassert ("MemoryModel::locals_push", mm.locals_offset () == lo + 32);
    mm.locals_pop (32);
    tap.tassert ("MemoryModel::locals_pop", mm.locals_offset () == lo);
    tap.tassert ("MemoryModel::locals_size", mm.locals_size () == 32);
  }

  tap.print_plan ();

  return 0;
}
