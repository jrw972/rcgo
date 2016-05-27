#ifndef RC_UTEST_VISITOR_HELPER_HPP
#define RC_UTEST_VISITOR_HELPER_HPP

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

#endif // RC_UTEST_VISITOR_HELPER_HPP
