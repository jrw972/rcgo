#include <iostream>

struct Tap {
  Tap () : test_number_ (0) { }
  void tassert (const char* description,
               bool ok)
  {
    ++test_number_;
    if (ok) {
      std::cout << "ok " << test_number_ << " - " << description << '\n';
    } else {
      std::cout << "not ok " << test_number_ << " - " << description << '\n';
    }
  }

  void print_plan () const
  {
    std::cout << "1.." << test_number_ << '\n';
  }

private:
  int test_number_;
};

#include "heap.hpp"

using namespace runtime;

struct link {
  int value;
  heap_t* heap;
  link* next;

  link () : heap (NULL), next (NULL) { }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    link root;
    heap_t* h = heap_make (&root, sizeof (link));
    tap.tassert ("heap_make: not null", h != NULL);
    tap.tassert ("heap_make: root is root", heap_root (h) == &root);
    tap.tassert ("heap_make: root not in heap", heap_contains (h, &root) == false);
  }

  {
    heap_t* h = heap_make_size (sizeof (link));
    link* root = static_cast<link*> (heap_root (h));
    tap.tassert ("heap_make_size: not null", h != NULL);
    tap.tassert ("heap_make_size: root is not null", root != NULL);
    tap.tassert ("heap_make_size: root is root", heap_root (h) == root);
    tap.tassert ("heap_make_size: root in heap", heap_contains (h, root) == true);
    tap.tassert ("heap_make_size: root is object", heap_is_object (h, root) == true);
    tap.tassert ("heap_make_size: root is allocated", heap_is_allocated (h, root) == true);
  }

  {
    link root;
    heap_t* h = heap_make (&root, sizeof (link));
    link* obj = static_cast<link*> (heap_allocate (h, sizeof (link)));
    tap.tassert ("heap_allocate: not null", obj != NULL);
    tap.tassert ("heap_allocate: obj in heap", heap_contains (h, obj) == true);
    tap.tassert ("heap_allocate: obj is object", heap_is_object (h, obj) == true);
    tap.tassert ("heap_allocate: obj is allocated", heap_is_allocated (h, obj) == true && heap_is_allocated (h, &obj->next) == true);
  }

  {
    link root;
    heap_t* h = heap_make (&root, sizeof (link));
    link* obj1 = static_cast<link*> (heap_allocate (h, sizeof (link)));
    link* obj2 = static_cast<link*> (heap_allocate (h, sizeof (link)));
    root.next = obj2;
    bool r = heap_collect_garbage (h, true);
    tap.tassert ("heap_collect_garbage: was performed", r == true);
    tap.tassert ("heap_collect_garbage: obj1 is collected", heap_contains (h, obj1) == false || heap_is_allocated (h, obj1) == false);
    tap.tassert ("heap_collect_garbage: obj2 is not collected", heap_contains (h, obj2) == true || heap_is_allocated (h, obj2) == true && heap_is_object (h, obj2));
  }

  {
    link root1;
    heap_t* h1 = heap_make (&root1, sizeof (link));
    heap_t* h2 = heap_make_size (sizeof (link));
    link* root2 = static_cast<link*> (heap_root (h2));
    link* obj = static_cast<link*> (heap_allocate (h2, sizeof (link)));
    heap_merge (h1, h2);
    tap.tassert ("heap_merge: obj in heap", heap_contains (h1, obj) == true);
    tap.tassert ("heap_merge: obj is object", heap_is_object (h1, obj) == true);
    tap.tassert ("heap_merge: obj is allocated", heap_is_allocated (h1, obj) == true);
    heap_collect_garbage (h1, true);
    tap.tassert ("heap_merge: obj is collected", heap_contains (h1, obj) == false || heap_is_allocated (h1, obj) == false);
  }

  {
    link root1;
    heap_t* h1 = heap_make (&root1, sizeof (link));
    heap_t* h2 = heap_make_size (sizeof (link));
    link* root2 = static_cast<link*> (heap_root (h2));
    link* obj = static_cast<link*> (heap_allocate (h2, sizeof (link)));
    root2->next = obj;
    heap_insert_child (h1, h2);
    tap.tassert ("heap_insert_child: child heap inserted", heap_is_child (h1, h2) == true);
    root1.heap = h2;
    heap_collect_garbage (h1, true);
    heap_collect_garbage (h2, true);
    tap.tassert ("heap_insert_child: obj in heap", heap_contains (h2, obj) == true);
    tap.tassert ("heap_insert_child: obj is object", heap_is_object (h2, obj) == true);
    tap.tassert ("heap_insert_child: obj is allocated", heap_is_allocated (h2, obj) == true);

    root1.heap = NULL;
    heap_collect_garbage (h1, true);
    tap.tassert ("heap_insert_child: child heap collected", heap_is_child (h1, h2) == false);
  }

  {
    link root1;
    heap_t* h1 = heap_make (&root1, sizeof (link));
    heap_t* h2 = heap_make_size (sizeof (link));
    link* root2 = static_cast<link*> (heap_root (h2));
    heap_insert_child (h1, h2);
    heap_remove_from_parent (h2);
    tap.tassert ("heap_remove_from_parent: child heap removed", heap_is_child (h1, h2) == false);
  }

  tap.print_plan ();

  return 0;
}
