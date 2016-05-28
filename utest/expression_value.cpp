#include "expression_value.hpp"

#include "tap.hpp"

using namespace semantic;
using namespace type;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    ExpressionValue ev1;
    ExpressionValue ev2;
    ev2.expression_kind = TypeExpressionKind;

    tap.tassert ("ExpressionValue::is_unknown",
                 ev1.is_unknown () == true &&
                 ev2.is_unknown () == false);
  }

  {
    ExpressionValue ev1;
    ExpressionValue ev2;
    ev2.expression_kind = TypeExpressionKind;

    tap.tassert ("ExpressionValue::is_type",
                 ev1.is_type () == false &&
                 ev2.is_type () == true);
  }

  {
    ExpressionValue ev1;
    ExpressionValue ev2;
    ExpressionValue ev3;
    ev2.expression_kind = ValueExpressionKind;
    ev3.expression_kind = VariableExpressionKind;

    tap.tassert ("ExpressionValue::is_value_or_variable",
                 ev1.is_value_or_variable () == false &&
                 ev2.is_value_or_variable () == true &&
                 ev3.is_value_or_variable () == true);
  }

  {
    ExpressionValue ev;
    ev.type = &named_string;
    ev.indirection_mutability = Mutable;
    ev.fix_string_indirection_mutability ();

    tap.tassert ("ExpressionValue::fix_string_indiration_mutability",
                 ev.indirection_mutability == Immutable);
  }

  {
    ExpressionValue ev;
    ev.type = UntypedBoolean::instance ();
    ev.value.present = true;
    ev.convert (&named_bool);

    tap.tassert ("ExpressionValue::convert",
                 ev.type == &named_bool);
  }

  tap.print_plan ();

  return 0;
}
