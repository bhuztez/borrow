#include "borrow.hpp"

int
main() {
  using ::borrow::borrow;

  int value = 1;
  BEGIN_LIFETIME($1);

  auto p = borrow<$1>(value);
  *p;
  static_assert(::std::is_same<decltype(*p), int const&>::value);

  END_LIFETIME($1);

  *p; // compiler error here
}
