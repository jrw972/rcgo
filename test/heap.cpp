#include <iostream>

struct Tap
{
  Tap () : test_number_ (0) { }
  void tassert (const char* description,
                bool ok)
  {
    ++test_number_;
    if (ok)
      {
        std::cout << "ok " << test_number_ << " - " << description << '\n';
      }
    else
      {
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

struct link
{
  int value;
  Heap* heap;
  link* next;

  link () : heap (NULL), next (NULL) { }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    link root;
    Heap* h = new Heap (&root, sizeof (link));
    tap.tassert ("Heap::Heap root is root", h->root () == &root);
    tap.tassert ("Heap::Heap root not in heap", h->contains (&root) == false);
  }

  {
    Heap* h = new Heap (sizeof (link));
    link* root = static_cast<link*> (h->root ());
    tap.tassert ("Heap::Heap root is not null", root != NULL);
    tap.tassert ("Heap::Heap root is root", h->root () == root);
    tap.tassert ("Heap::Heap root in heap", h->contains (root) == true);
    tap.tassert ("Heap::Heap root is object", h->is_object (root) == true);
    tap.tassert ("Heap::Heap root is allocated", h->is_allocated (root) == true);
  }

  {
    link root;
    Heap* h = new Heap (&root, sizeof (link));
    link* obj = static_cast<link*> (h->allocate (sizeof (link)));
    tap.tassert ("Heap::allocate not null", obj != NULL);
    tap.tassert ("Heap::allocate obj in heap", h->contains (obj) == true);
    tap.tassert ("Heap::allocate obj is object", h->is_object (obj) == true);
    tap.tassert ("Heap::allocate obj is allocated", h->is_allocated (obj) == true && h->is_allocated (&obj->next) == true);
  }

  {
    link root;
    Heap* h = new Heap (&root, sizeof (link));
    link* obj1 = static_cast<link*> (h->allocate (sizeof (link)));
    link* obj2 = static_cast<link*> (h->allocate (sizeof (link)));
    root.next = obj2;
    bool r = h->collect_garbage (true);
    tap.tassert ("Heap::collect_garbage was performed", r == true);
    tap.tassert ("Heap::collect_garbage obj1 is collected", h->contains (obj1) == false || h->is_allocated (obj1) == false);
    tap.tassert ("Heap::collect_garbage obj2 is not collected", h->contains (obj2) == true || h->is_allocated (obj2) == true && h->is_object (obj2));
  }

  {
    link root1;
    Heap* h1 = new Heap (&root1, sizeof (link));
    Heap* h2 = new Heap (sizeof (link));
    link* root2 = static_cast<link*> (h2->root ());
    link* obj = static_cast<link*> (h2->allocate (sizeof (link)));
    h1->merge (h2);
    tap.tassert ("Heap::merge obj in heap", h1->contains (obj) == true);
    tap.tassert ("Heap::merge obj is object", h1->is_object (obj) == true);
    tap.tassert ("Heap::merge obj is allocated", h1->is_allocated (obj) == true);
    h1->collect_garbage (true);
    tap.tassert ("Heap::merge obj is collected", h1->contains (obj) == false || h1->is_allocated (obj) == false);
  }

  {
    link root1;
    Heap* h1 = new Heap (&root1, sizeof (link));
    Heap* h2 = new Heap (sizeof (link));
    link* root2 = static_cast<link*> (h2->root ());
    link* obj = static_cast<link*> (h2->allocate (sizeof (link)));
    root2->next = obj;
    h1->insert_child (h2);
    tap.tassert ("Heap::insert_child child heap inserted", h1->is_child (h2) == true);
    root1.heap = h2;
    h1->collect_garbage (true);
    h2->collect_garbage (true);
    tap.tassert ("Heap::insert_child obj in heap", h2->contains (obj) == true);
    tap.tassert ("Heap::insert_child obj is object", h2->is_object (obj) == true);
    tap.tassert ("Heap::insert_child obj is allocated", h2->is_allocated (obj) == true);

    root1.heap = NULL;
    h1->collect_garbage (true);
    tap.tassert ("Heap::insert_child child heap collected", h1->is_child (h2) == false);
  }

  {
    link root1;
    Heap* h1 = new Heap (&root1, sizeof (link));
    Heap* h2 = new Heap (sizeof (link));
    link* root2 = static_cast<link*> (h2->root ());
    h1->insert_child (h2);
    h2->remove_from_parent ();
    tap.tassert ("Heap::remove_from_parent child heap removed", h1->is_child (h2) == false);
  }

  tap.print_plan ();

  return 0;
}
