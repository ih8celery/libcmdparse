/**
 * \file option.cpp
 *
 * \author Adam Marshall (ih8celery)
 *
 * \brief parse command line arguments/options
 */
#include "option.h"

namespace cli {
  Option::Option(): number(Property::Number::ZERO_ONE),
                    assignment(Property::Assignment::NO_ASSIGN),
                    collection(Property::Collection::SCALAR),
                    type(Property::Arg_Type::STRING) {}

  bool operator<(const Option& l, const Option& r) noexcept {
    return (l.name < r.name);
  }

  bool operator==(const Option& l, const Option& r) noexcept {
    return (l.name == r.name);
  }
}
