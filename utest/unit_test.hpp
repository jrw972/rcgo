#ifndef RCGO_UTEST_UNIT_TEST_HPP
#define RCGO_UTEST_UNIT_TEST_HPP

#include <string>
#include <iostream>

struct Tree;

struct UnitTester
{
  virtual void register_test_unit (void (*func) (), const char* name) = 0;
  virtual void register_test_scenario (void (*func) (), const char* name) = 0;
  virtual int run_tests (bool dry_run) = 0;
  virtual int run_tests (bool dry_run, Tree* tree) = 0;
  virtual void error (const char* condition, const char* file, int line) = 0;

  static UnitTester* impl ();
};

struct RegisterTestUnit
{
  RegisterTestUnit (void (*func) (), const char* name)
  {
    UnitTester::impl ()->register_test_unit (func, name);
  }
};

struct RegisterTestScenario
{
  RegisterTestScenario (void (*func) (), const char* name)
  {
    UnitTester::impl ()->register_test_scenario (func, name);
  }
};

#define ASSERT(condition) do { if (!(condition)) { UnitTester::impl ()->error (#condition, __FILE__, __LINE__); } } while (0);

#define ASSERT_NO_ERROR(er) do { ASSERT (er.list ().empty ()); } while (0);

#define ASSERT_LAST_ERROR(er, code) do { ASSERT (!er.list ().empty () && er.list ().back () == code); } while (0);

#endif /* RCGO_UTEST_UNIT_TYPE_HPP */
