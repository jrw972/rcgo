#include "unit_test.hpp"

#include <getopt.h>

#include <vector>
#include <map>
#include <numeric>
#include <cstdlib>
#include <cstring>
#include <algorithm>

struct comma_joiner
{
  std::string
  operator() (const std::string& x, const std::string& y) const
  {
    if (x.empty ())
      {
        return y;
      }
    else
      {
        return x + "," + y;
      }
  }
};

enum Op
{
  Or,
  And,
  Not,
  Unit,
  Scenario
};

struct Tree
{
  Tree (Op a_op, const std::string& s)
    : op (a_op)
    , left (NULL)
    , right (NULL)
    , id (s)
  { }

  Tree (Op a_op, Tree* a_left)
    : op (a_op)
    , left (a_left)
    , right (NULL)
  { }

  Tree (Op a_op, Tree* a_left, Tree* a_right)
    : op (a_op)
    , left (a_left)
    , right (a_right)
  { }

  Op const op;
  Tree* const left;
  Tree* const right;
  std::string const id;

  size_t worst_case;
};

struct UnitTesterImpl : public UnitTester
{
  UnitTesterImpl ()
  { }

  void register_test_unit (void (*func) (), const char* name)
  {
    func_to_unit_.insert (std::make_pair (func, name));
    std::pair<StringMapType::iterator, bool> x = unit_to_funcs_.insert (std::make_pair (name, std::vector<void (*) () > ()));
    x.first->second.push_back (func);
  }

  void register_test_scenario (void (*func) (), const char* name)
  {
    std::pair<FuncToScenariosMapType::iterator, bool> x = func_to_scenarios_.insert (std::make_pair (func, std::vector<std::string> ()));
    x.first->second.push_back (name);

    std::pair<StringMapType::iterator, bool> y = scenario_to_funcs_.insert (std::make_pair (name, std::vector<void (*) () > ()));
    y.first->second.push_back (func);
  }

  int run_tests (bool dry_run)
  {
    int retval = EXIT_SUCCESS;
    size_t test_number = 0;

    // Produce TAP output.  Can change this to a more useful format later.
    for (FuncToUnitMapType::const_iterator pos = func_to_unit_.begin (), limit = func_to_unit_.end ();
         pos != limit;
         ++pos)
      {
        unit_ = pos->second;
        scenarios_.clear ();
        FuncToScenariosMapType::const_iterator p = func_to_scenarios_.find (pos->first);
        if (p != func_to_scenarios_.end ())
          {
            scenarios_ = std::accumulate (p->second.begin (), p->second.end (), std::string (), comma_joiner ());
          }
        test_fail_ = false;
        ++test_number;
        if (!dry_run)
          {
            (pos->first) ();
          }
        if (test_fail_)
          {
            std::cout << "not ok " << test_number << " - " << unit_ << " scenarios=" << scenarios_ << "\n";
            retval = EXIT_FAILURE;
          }
        else
          {
            std::cout << "ok " << test_number << " - " << unit_ << " scenarios=" << scenarios_ << "\n";
          }
      }

    std::cout << "1.." << test_number << '\n';

    return retval;
  }

  int run_tests (bool dry_run, Tree* tree)
  {
    // First, sort and remove duplicates.
    for (FuncToScenariosMapType::iterator pos = func_to_scenarios_.begin (),
         limit = func_to_scenarios_.end ();
         pos != limit;
         ++pos)
      {
        std::sort (pos->second.begin (), pos->second.end ());
        pos->second.erase (std::unique (pos->second.begin (), pos->second.end ()), pos->second.end ());
      }

    for (StringMapType::iterator pos = unit_to_funcs_.begin (),
         limit = unit_to_funcs_.end ();
         pos != limit;
         ++pos)
      {
        std::sort (pos->second.begin (), pos->second.end ());
        pos->second.erase (std::unique (pos->second.begin (), pos->second.end ()), pos->second.end ());
      }

    for (StringMapType::iterator pos = scenario_to_funcs_.begin (),
         limit = scenario_to_funcs_.end ();
         pos != limit;
         ++pos)
      {
        std::sort (pos->second.begin (), pos->second.end ());
        pos->second.erase (std::unique (pos->second.begin (), pos->second.end ()), pos->second.end ());
      }

    // Do worst-case analysis.
    analyze (tree);
    // It would be sweet if we had an optimization pass.
    // optmize (tree);

    // Build the set of functions to execute.
    FuncVector x;
    evaluate (tree, x);

    // Create indexes.
    int retval = EXIT_SUCCESS;
    size_t test_number = 0;

    // Produce TAP output.  Can change this to a more useful format later.
    for (FuncVector::const_iterator pos = x.begin (), limit = x.end ();
         pos != limit;
         ++pos)
      {
        unit_ = func_to_unit_[*pos];
        scenarios_.clear ();
        FuncToScenariosMapType::const_iterator p = func_to_scenarios_.find (*pos);
        if (p != func_to_scenarios_.end ())
          {
            scenarios_ = std::accumulate (p->second.begin (), p->second.end (), std::string (), comma_joiner ());
          }
        test_fail_ = false;
        ++test_number;
        if (!dry_run)
          {
            (*pos) ();
          }
        if (test_fail_)
          {
            std::cout << "not ok " << test_number << " - " << unit_ << " scenarios=" << scenarios_ << "\n";
            retval = EXIT_FAILURE;
          }
        else
          {
            std::cout << "ok " << test_number << " - " << unit_ << " scenarios=" << scenarios_ << "\n";
          }
      }

    std::cout << "1.." << test_number << '\n';

    return retval;
  }

  void error (const char* condition, const char* file, int line)
  {
    std::cout << file << ':' << line << ": assertion failed: " << condition << " (unit=" << unit_ << ",scenarios=" << scenarios_ << ")\n";
    test_fail_ = true;
  }

  // Determine the maximum number of results for a tree.
  void analyze (Tree* tree) const
  {
    if (tree->left)
      {
        analyze (tree->left);
      }
    if (tree->right)
      {
        analyze (tree->right);
      }
    switch (tree->op)
      {
      case Or:
        tree->worst_case = std::max (tree->left->worst_case, tree->right->worst_case);
        break;
      case And:
        tree->worst_case = std::min (tree->left->worst_case, tree->right->worst_case);
        break;
      case Not:
        tree->worst_case = func_to_unit_.size () - tree->left->worst_case;
        break;
      case Unit:
      {
        StringMapType::const_iterator pos = unit_to_funcs_.lower_bound (tree->id);
        const size_t s = tree->id.size ();
        size_t count = 0;
        while (pos != unit_to_funcs_.end () &&
               pos->first.size () >= s &&
               pos->first.substr (0, s) == tree->id)
          {
            count += pos->second.size ();
            ++pos;
          }
        tree->worst_case = count;
      }
      break;
      case Scenario:
      {
        StringMapType::const_iterator pos = scenario_to_funcs_.find (tree->id);
        if (pos != scenario_to_funcs_.end ())
          {
            tree->worst_case = pos->second.size ();
          }
        else
          {
            tree->worst_case = 0;
          }
      }
      break;
      }
  }

  typedef std::vector<void (*) () > FuncVector;

  // Evaluate a tree producing a vector of functions to execute.
  void evaluate (Tree* tree, FuncVector& out) const
  {
    switch (tree->op)
      {
      case Or:
        evaluate (tree->left, out);
        evaluate (tree->right, out);
        std::sort (out.begin (), out.end ());
        out.erase (std::unique (out.begin (), out.end ()), out.end ());
        break;
      case And:
        if (tree->left->worst_case < tree->right->worst_case)
          {
            evaluate (tree->left, out);
            for (FuncVector::iterator pos = out.begin (), limit = out.end ();
                 pos != limit;
                )
              {
                if (!check (tree->right, *pos))
                  {
                    out.erase (pos++);
                  }
                else
                  {
                    ++pos;
                  }
              }
          }
        else
          {
            evaluate (tree->right, out);
            for (FuncVector::iterator pos = out.begin (), limit = out.end ();
                 pos != limit;
                )
              {
                if (!check (tree->left, *pos))
                  {
                    out.erase (pos++);
                  }
                else
                  {
                    ++pos;
                  }
              }
          }
        break;
      case Not:
      {
        for (FuncToUnitMapType::const_iterator pos = func_to_unit_.begin (),
             limit = func_to_unit_.end ();
             pos != limit;
             ++pos)
          {
            if (!check (tree->left, pos->first))
              {
                out.push_back (pos->first);
              }
          }
      }
      break;
      case Unit:
      {
        StringMapType::const_iterator pos = unit_to_funcs_.lower_bound (tree->id);
        const size_t s = tree->id.size ();
        while (pos != unit_to_funcs_.end () &&
               pos->first.size () >= s &&
               pos->first.substr (0, s) == tree->id)
          {
            out.insert (out.end (), pos->second.begin (), pos->second.end ());
            ++pos;
          }
      }
      break;
      case Scenario:
      {
        StringMapType::const_iterator pos = scenario_to_funcs_.find (tree->id);
        if (pos != scenario_to_funcs_.end ())
          {
            out.insert (out.end (), pos->second.begin (), pos->second.end ());
          }
      }
      break;
      }
  }


  // Check that a function is included by the tree.
  bool check (Tree* tree, void (*func) ()) const
  {
    switch (tree->op)
      {
      case Or:
        // Succeed fast.
        if (tree->left->worst_case > tree->right->worst_case)
          {
            return check (tree->left, func) || check (tree->right, func);
          }
        else
          {
            return check (tree->right, func) || check (tree->left, func);
          }
        break;
      case And:
        // Fail fast.
        if (tree->left->worst_case < tree->right->worst_case)
          {
            return check (tree->left, func) && check (tree->right, func);
          }
        else
          {
            return check (tree->right, func) && check (tree->left, func);
          }
        break;
      case Not:
        return !check (tree->left, func);
        break;
      case Unit:
      {
        const std::size_t s = tree->id.size ();
        FuncToUnitMapType::const_iterator p = func_to_unit_.find (func);
        return p != func_to_unit_.end () && p->second.size () >= s && p->second.substr (0, s) == tree->id;
      }
      break;
      case Scenario:
      {
        FuncToScenariosMapType::const_iterator p = func_to_scenarios_.find (func);
        return p != func_to_scenarios_.end () && std::find (p->second.begin (), p->second.end (), tree->id) != p->second.end ();
      }
      break;
      }

    return false;
  }

  typedef std::map<void (*) (), std::string> FuncToUnitMapType;
  FuncToUnitMapType func_to_unit_;

  typedef std::map<void (*) (), std::vector<std::string> > FuncToScenariosMapType;
  FuncToScenariosMapType func_to_scenarios_;

  typedef std::map<std::string, FuncVector > StringMapType;
  StringMapType unit_to_funcs_;
  StringMapType scenario_to_funcs_;

  std::string unit_;
  std::string scenarios_;
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

void consume_white_space (const char*& pos, const char* limit)
{
  while (pos != limit && *pos == ' ') ++pos;
}

bool next_char_is (const char* pos, const char* limit, char c)
{
  return pos != limit && *pos == c;
}

Tree* parse_or_expression (const char*& pos, const char* limit);

Tree* parse_unit (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);

  std::string u;
  while (pos != limit &&
         (isalnum (*pos) || *pos == '_' || *pos == ':'))
    {
      u += *pos++;
    }
  if (u.empty ())
    {
      std::cerr << "Empty unit" << std::endl;
    }
  return new Tree (Unit, u);
}

Tree* parse_scenario (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);

  std::string u;
  while (pos != limit &&
         (isalnum (*pos) || *pos == '_'))
    {
      u += *pos++;
    }
  if (u.empty ())
    {
      std::cerr << "Empty scenario" << std::endl;
    }
  return new Tree (Scenario, u);
}

Tree* parse_term (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);
  if (next_char_is (pos, limit, '('))
    {
      ++pos;
      Tree* t = parse_or_expression (pos, limit);
      consume_white_space (pos, limit);
      if (next_char_is (pos, limit, ')'))
        {
          ++pos;
          return t;
        }
      else
        {
          std::cerr << "Expected ')'" << std::endl;
          return t;
        }
    }
  else if (next_char_is (pos, limit, '$'))
    {
      ++pos;
      return parse_scenario (pos, limit);
    }
  else
    {
      return parse_unit (pos, limit);
    }
}

Tree* parse_not_expression (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);
  if (next_char_is (pos, limit, '!'))
    {
      ++pos;
      Tree* t = parse_not_expression (pos, limit);
      return new Tree (Not, t);
    }
  return parse_term (pos, limit);
}

Tree* parse_and_expression (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);
  Tree* t = parse_not_expression (pos, limit);
  consume_white_space (pos, limit);
  if (next_char_is (pos, limit, '&'))
    {
      ++pos;
      Tree* t2 = parse_and_expression (pos, limit);
      t = new Tree (And, t, t2);
    }
  return t;
}

Tree* parse_or_expression (const char*& pos, const char* limit)
{
  consume_white_space (pos, limit);
  Tree* t = parse_and_expression (pos, limit);
  consume_white_space (pos, limit);
  if (next_char_is (pos, limit, '|'))
    {
      ++pos;
      Tree* t2 = parse_or_expression (pos, limit);
      t = new Tree (Or, t, t2);
    }
  return t;
}

Tree* parse_root_expression (const char*& pos, const char* limit)
{
  Tree* t = parse_or_expression (pos, limit);
  consume_white_space (pos, limit);
  if (pos != limit)
    {
      std::cerr << "Ignoring trailing characters in expression: " << pos << std::endl;
    }
  return t;
}

static void
try_help (void)
{
  std::cerr << "Try --help' for more information.\n";
  exit (EXIT_FAILURE);
}

int main (int argc, char** argv)
{
  bool dry_run = false;

  while (true)
    {
      static struct option long_options[] =
      {
        {"help",        no_argument, NULL, 'h'},

        {"dry-run",     no_argument, NULL, 'd'},

        {0, 0, 0, 0}
      };

      int c = getopt_long (argc, argv, "hd", long_options, NULL);

      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          break;
        case 'h':
          std::cout << "Usage: " << argv[0] << " { OPTION } [EXPRESSION]\n"
                    <<
                    "Run unit tests.\n"
                    "\n"
                    "  --dry-run       show which tests will be run by EXPRESSION\n"
                    "  -h, --help      display this help and exit\n"
                    "\n"
                    "EXPRESSION selects zero or more tests to run."
                    "\n"
                    "  Expression    = [ OrExpression ] .\n"
                    "  OrExpression  = AndExpression [ '|' OrExpression ] .\n"
                    "  AndExpression = NotExpression [ '&' AndExpression ] .\n"
                    "  NotExpression = Term | '!' NotExpression .\n"
                    "  Term          = unit | scenario | '(' OrExpression ')' .\n"
                    "\n"
                    "A unit is regular or scoped identifier, e.g., ast::Node::Node.\n"
                    "A scenario is an identifier prefixed with $, e.g., $process_undefined_type.\n"
                    "An empty expression means run all tests.\n"
                    ;
          exit (EXIT_SUCCESS);
          break;

        case 'd':
          dry_run = true;
          break;

        default:
          try_help ();
          break;
        }
    }

  if (optind == argc)
    {
      // Run all tests.
      return UnitTester::impl ()->run_tests (dry_run);
    }
  else if (optind + 1 == argc)
    {
      const char* pos = argv[optind];
      Tree* t = parse_root_expression (pos, pos + strlen (argv[optind]));
      return UnitTester::impl ()->run_tests (dry_run, t);
    }
  else
    {
      std::cerr << "Too many arguments.\n";
      try_help ();
    }
}
