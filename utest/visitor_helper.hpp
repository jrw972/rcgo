#ifndef RC_UTEST_VISITOR_HELPER_HPP
#define RC_UTEST_VISITOR_HELPER_HPP

#include "unit_test.hpp"

template <typename V, typename T>
struct TestVisitor : public V
{
  T* item;

  TestVisitor () : item (NULL) { }

  void visit (T& item)
  {
    this->item = &item;
  }

};

template <typename V, typename T>
struct TestConstVisitor : public V
{
  const T* item;

  TestConstVisitor () : item (NULL) { }

  void visit (const T& item)
  {
    this->item = &item;
  }

};

template <typename V, typename T>
struct TestDefaultVisitor : public V
{
  T* item;

  TestDefaultVisitor () : item (NULL) { }

  void default_action (T& item)
  {
    this->item = &item;
  }
};

template <typename V, typename T>
struct TestDefaultConstVisitor : public V
{
  const T* item;

  TestDefaultConstVisitor () : item (NULL) { }

  void default_action (const T& item)
  {
    this->item = &item;
  }
};

template <typename Visitor, typename Node>
inline void test_accept (Node& node)
{
  TestVisitor<Visitor, Node> v;
  node.accept (v);
  ASSERT (v.item == &node);
}


#endif // RC_UTEST_VISITOR_HELPER_HPP
