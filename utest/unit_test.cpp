#include "unit_test.hpp"

#include <map>

struct Unit
{
  std::string name;
  std::string scenario;
};

struct UnitTesterImpl : public UnitTester
{
  UnitTesterImpl ()
  { }

  void register_test_unit (void (*func) (), const char* name)
  {
    std::pair<MapType::iterator, bool> x = map_.insert (std::make_pair (func, Unit ()));
    x.first->second.name = name;
  }

  void register_test_scenario (void (*func) (), const char* name)
  {
    std::pair<MapType::iterator, bool> x = map_.insert (std::make_pair (func, Unit ()));
    x.first->second.scenario = name;
  }

  int run_tests ()
  {
    int retval = 0;
    size_t test_number = 0;

    // Produce TAP output.  Can change this to a more useful format later.
    for (MapType::const_iterator pos = map_.begin (), limit = map_.end ();
         pos != limit;
         ++pos)
      {
        unit_ = pos->second.name;
        scenario_ = pos->second.scenario;
        test_fail_ = false;
        ++test_number;
        (pos->first) ();
        if (test_fail_)
          {
            std::cout << "not ok " << test_number << " - " << unit_ << " (" << scenario_ << ")\n";
          }
        else
          {
            std::cout << "ok " << test_number << " - " << unit_ << " (" << scenario_ << ")\n";
          }
      }

    std::cout << "1.." << test_number << '\n';

    return retval;
  }

  void error (const char* condition, const char* file, int line)
  {
    std::cout << file << ':' << line << ": assertion failed: " << condition << " (unit=" << unit_ << ",scenario=" << scenario_ << ")\n";
    test_fail_ = true;
  }

  typedef std::map<void (*) (), Unit> MapType;
  MapType map_;
  std::string unit_;
  std::string scenario_;
  bool test_fail_;
};

UnitTester* UnitTester::impl ()
{
  static UnitTester* _impl = NULL;
  if (_impl == NULL)
    {
      _impl = new UnitTesterImpl ();
    }
  return _impl;
}

int main (int argc, char** argv)
{
  return UnitTester::impl ()->run_tests ();
}
