#include "memory_model.hpp"

#include "tap.hpp"
#include "type.hpp"

using namespace runtime;

int
main (int argc, char** argv)
{
  Tap tap;

  arch::set_stack_alignment (8);

  {
    MemoryModel mm;
    tap.tassert ("MemoryModel::MemoryModel", mm.arguments_empty () && mm.locals_empty ());
  }

  {
    MemoryModel mm;
    ptrdiff_t ao = mm.arguments_offset ();
    mm.arguments_push (&type::named_int32);
    tap.tassert ("MemoryModel::arguments_push", mm.arguments_offset () == ao - 8);
    mm.set_receiver_offset ();
    tap.tassert ("MemoryModel::arguments_push", mm.receiver_offset () == ao - 8);
  }

  {
    MemoryModel mm;
    ptrdiff_t lo = mm.locals_offset ();
    mm.locals_push (&type::named_int);
    tap.tassert ("MemoryModel::locals_push", mm.locals_offset () == lo + 8);
    mm.locals_pop (4);
    tap.tassert ("MemoryModel::locals_pop", mm.locals_offset () == lo);
    tap.tassert ("MemoryModel::locals_size", mm.locals_size_on_stack () == 8);
  }

  tap.print_plan ();

  return 0;
}
