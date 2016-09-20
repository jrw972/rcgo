#include "heap.hpp"

#include <algorithm>

#include "tap.hpp"

using namespace runtime;

struct Link
{
  int value;
  Heap* heap;
  Link* next;

  Link () : heap (NULL), next (NULL) { }
};

bool BlockSorter (const Block* x, const Block* y)
{
  return x->begin () < y->begin ();
}

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    tap.tassert ("Heap::Heap root is root", h->root () == &root);
    tap.tassert ("Heap::Heap root not in heap", h->contains (&root) == false);
    delete h;
  }

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    tap.tassert ("Heap::is_object NULL", h->is_object (NULL) == false);
    tap.tassert ("Heap::is_allocated NULL", h->is_allocated (NULL) == false);
    delete h;
  }

  {
    Heap* h = new Heap (sizeof (Link));
    Link* root = static_cast<Link*> (h->root ());
    tap.tassert ("Heap::Heap root is not null", root != NULL);
    tap.tassert ("Heap::Heap root is root", h->root () == root);
    tap.tassert ("Heap::Heap root in heap", h->contains (root) == true);
    tap.tassert ("Heap::Heap root is object", h->is_object (root) == true);
    tap.tassert ("Heap::Heap root is allocated", h->is_allocated (root) == true);
    delete h;
  }

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    Link* obj = static_cast<Link*> (h->allocate (sizeof (Link)));
    tap.tassert ("Heap::allocate not null", obj != NULL);
    tap.tassert ("Heap::allocate obj in heap", h->contains (obj) == true);
    tap.tassert ("Heap::allocate obj is object", h->is_object (obj) == true);
    tap.tassert ("Heap::allocate obj is allocated", h->is_allocated (obj) == true && h->is_allocated (&obj->next) == true);
    delete h;
  }

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    Link* obj = static_cast<Link*> (h->allocate (0));
    tap.tassert ("Heap::allocate 0 -> NULL", obj == NULL);
    delete h;
  }

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    h->allocate (sizeof (Link));
    h->allocate (sizeof (Link));
    h->allocate (sizeof (Link));
    h->allocate (sizeof (Link));
    Link* obj5 = static_cast<Link*> (h->allocate (sizeof (Link)));
    root.next = obj5;
    h->collect_garbage (true);
    Link* arr = static_cast<Link*> (h->allocate (64 * sizeof (Link)));
    tap.tassert ("Heap::allocate traverse free list", arr != NULL);
    delete h;
  }

  {
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    Link* obj1 = static_cast<Link*> (h->allocate (sizeof (Link)));
    Link* obj2 = static_cast<Link*> (h->allocate (sizeof (Link)));
    root.next = obj2;
    bool r = h->collect_garbage (true);
    tap.tassert ("Heap::collect_garbage was performed", r == true);
    tap.tassert ("Heap::collect_garbage obj1 is collected", h->contains (obj1) == false || h->is_allocated (obj1) == false);
    tap.tassert ("Heap::collect_garbage obj2 is not collected", h->contains (obj2) == true || (h->is_allocated (obj2) == true && h->is_object (obj2)));
    delete h;
  }

  {
    Link root1;
    Heap* h1 = new Heap (&root1, sizeof (Link));
    Heap* h2 = new Heap (sizeof (Link));
    h1->insert_child (h2);
    tap.tassert ("Heap::collect_garbage of children", h1->collect_garbage ());
    delete h1;
  }

  {
    Link root1;
    Heap* h1 = new Heap (&root1, sizeof (Link));
    Heap* h2 = new Heap (sizeof (Link));
    Link* obj = static_cast<Link*> (h2->allocate (sizeof (Link)));
    h1->merge (h2);
    tap.tassert ("Heap::merge obj in heap", h1->contains (obj) == true);
    tap.tassert ("Heap::merge obj is object", h1->is_object (obj) == true);
    tap.tassert ("Heap::merge obj is allocated", h1->is_allocated (obj) == true);
    h1->collect_garbage (true);
    tap.tassert ("Heap::merge obj is collected", h1->contains (obj) == false || h1->is_allocated (obj) == false);
    delete h1;
  }

  {
    Link root1;
    Heap* h1 = new Heap (&root1, sizeof (Link));
    Heap* h2 = new Heap (sizeof (Link));
    Link* root2 = static_cast<Link*> (h2->root ());
    Link* obj = static_cast<Link*> (h2->allocate (sizeof (Link)));
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
    delete h1;
  }

  {
    Link root1;
    Heap* h1 = new Heap (&root1, sizeof (Link));
    Heap* h2 = new Heap (sizeof (Link));
    Heap* h3 = new Heap (sizeof (Link));
    h1->insert_child (h2);
    h1->insert_child (h3);
    h2->remove_from_parent ();
    tap.tassert ("Heap::remove_from_parent child heap removed", h1->is_child (h2) == false);
    delete h1;
  }

  // Tests for better coverage.
  {
    // Collect garbage in children.
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    Link* obj1 = static_cast<Link*> (h->allocate (5 * sizeof (Link)));
    Link* obj2 = static_cast<Link*> (h->allocate (sizeof (Link)));
    root.next = obj2;
    h->collect_garbage (true);
    obj1 = static_cast<Link*> (h->allocate (sizeof (Link)));
    delete h;
  }

  {
    // Check structure with internal pointers.
    Link root;
    Heap* h = new Heap (&root, sizeof (Link));
    Link* obj1 = static_cast<Link*> (h->allocate (5 * sizeof (Link)));
    Link* obj2 = static_cast<Link*> (h->allocate (sizeof (Link)));
    root.next = obj2;
    obj2->next = &obj1[2];
    h->collect_garbage (true);
    delete h;
  }

  {
    // Create a simple tree and delete it.
    std::vector<Block*> blocks;
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    std::sort (blocks.begin (), blocks.end (), BlockSorter);
    Block* root = blocks[1];
    Block::insert (&root, blocks[0]);
    Block::insert (&root, blocks[2]);
    delete root;
  }

  {
    // Mark an unallocated region.
    Block* wl = NULL;
    Block* root = Block::make (1);
    root->mark (root->begin (), &wl);
    delete root;
  }

  {
    // Sweep the root but retain the left child.
    std::vector<Block*> blocks;
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    std::sort (blocks.begin (), blocks.end (), BlockSorter);
    Block* child = blocks[0];
    Block* root = blocks[1];
    Block::insert (&root, child);
    Chunk c;
    Chunk* head = &c;
    child->set_mark ();
    Block::sweep (&root, &head);
    tap.tassert ("Block::sweep retain left", root == child);
    delete root;
  }

  {
    // Sweep the root but retain the children.
    std::vector<Block*> blocks;
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    blocks.push_back (Block::make (1));
    std::sort (blocks.begin (), blocks.end (), BlockSorter);
    Block* root = blocks[2];
    Block::insert (&root, blocks[0]);
    Block::insert (&root, blocks[3]);
    Block::insert (&root, blocks[1]);
    Chunk c;
    Chunk* head = &c;
    blocks[0]->set_mark ();
    blocks[1]->set_mark ();
    blocks[3]->set_mark ();
    Block::sweep (&root, &head);
    tap.tassert ("Block::sweep retain children", root == blocks[1]);
    delete root;
  }

  tap.print_plan ();

  return 0;
}
