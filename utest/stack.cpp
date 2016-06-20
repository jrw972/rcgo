#include "stack.hpp"

#include "tap.hpp"
#include <sstream>

using namespace runtime;

int
main (int argc, char** argv)
{
  Tap tap;
  arch::set_stack_alignment (8);

  {
    Stack s (1024);
    s.push_pointer (&s);
    void* p1 = s.peek_pointer ();
    void* p2 = s.pop_pointer ();
    tap.tassert ("Stack::push/pop/peek_pointer ()", p1 == &s && p2 == &s && s.empty ());
  }

  {
    Stack s (1024);
    int const x = 5;
    s.push (x);
    int y;
    s.pop (y);
    tap.tassert ("Stack::push/pop ()", x == y && s.empty ());
  }

  {
    Stack s (1024);
    int const x = 5;
    s.push (x);
    s.popn (8);
    tap.tassert ("Stack::popn ()", s.empty ());
  }

  {
    Stack s (1024);
    void* p = s.get_address (0);
    s.push_address (0);
    void* q = s.pop_pointer ();
    tap.tassert ("Stack::get/push_address ()", p == q);
  }

  {
    Stack s (1024);
    s.reserve (32);
    tap.tassert ("Stack::reserve ()", s.size () == 32);
  }

  {
    Stack s (1024);
    int const x = 5;
    s.load (&x, sizeof (x));
    int y;
    s.pop (y);
    tap.tassert ("Stack::load ()", x == y);
  }

  {
    Stack s (1024);
    int const x = 5;
    s.push (x);
    int y;
    s.store (&y, sizeof (y));
    tap.tassert ("Stack::store ()", x == y);
  }

  {
    Stack s (1024);
    s.setup (8);
    int const x = 5;
    s.write (0, &x, sizeof (x));
    int y;
    s.read (0, &y, sizeof (y));
    tap.tassert ("Stack::write/read ()", x == y);
  }

  {
    Stack s (1024);
    s.setup (8);
    void* x = &s;
    s.write (0, &x, sizeof (x));
    void* y = s.read_pointer (0);
    tap.tassert ("Stack::read_pointer ()", x == y);
  }

  {
    Stack s (1024);
    s.setup (8);
    int const x = 5;
    s.push (x);
    s.move (0, 8);
    int y;
    s.read (0, &y, sizeof (y));
    tap.tassert ("Stack::move ()", x == y);
  }

  {
    Stack s (1024);
    s.setup (8);
    int const x = 5;
    s.write (0, &x, sizeof (x));
    s.clear (0, sizeof (x));
    int y;
    s.read (0, &y, sizeof (y));
    tap.tassert ("Stack::clear ()", y == 0);
  }

  {
    Stack s (1024);
    char* old_bp = s.base_pointer ();
    s.setup (8);
    char* new_bp = s.base_pointer ();
    int y;
    s.read (8, &y, sizeof (y));
    tap.tassert ("Stack::setup ()", old_bp != new_bp && s.size () == 16 && y == 0);
  }

  {
    Stack s (1024);
    char* old_bp = s.base_pointer ();
    s.setup (8);
    s.teardown ();
    char* new_bp = s.base_pointer ();
    tap.tassert ("Stack::teardown ()", old_bp == new_bp);
  }

  {
    Stack s (1024);
    char c;
    s.base_pointer (&c);
    tap.tassert ("Stack::base_pointer ()", s.base_pointer () == &c);
  }

  {
    Stack s (1024);
    char* top_before = s.top ();
    s.reserve (8);
    char* top_after = s.top ();
    tap.tassert ("Stack::top ()", top_after - top_before == 8);
  }

  {
    Stack s (1024);
    void* ip = &s;
    s.push_pointer (ip);
    s.setup (0);
    void* pip = s.pointer_to_instruction_pointer ();
    tap.tassert ("Stack::pointer_to_instruction_pointer ()", *static_cast<char**> (pip) == ip);
  }

  {
    Stack s (1024);
    s.setup (8);
    std::stringstream ss;
    s.print (ss);
    tap.tassert ("Stack::print ()", ss.str () != "");
  }

  tap.print_plan ();

  return 0;
}
