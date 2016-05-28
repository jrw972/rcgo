#ifndef RC_UTEST_TAP_HPP
#define RC_UTEST_TAP_HPP

struct Tap
{
  Tap () : test_number_ (0) { }
  void tassert (const std::string& description,
                bool ok,
                const std::string& msg = std::string ())
  {
    ++test_number_;
    if (ok)
      {
        std::cout << "ok " << test_number_ << " - " << description << '\n';
      }
    else
      {
        std::cout << "not ok " << test_number_ << " - " << description << ' ' << msg << '\n';
      }
  }

  void print_plan () const
  {
    std::cout << "1.." << test_number_ << '\n';
  }

private:
  int test_number_;
};

#endif // RC_UTEST_TAP_HPP
