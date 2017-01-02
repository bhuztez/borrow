#include "borrow.hpp"

int
main() {
  using ::borrow::borrow_mut;

  int value = 1;
  BEGIN_LIFETIME($1);

  auto p = borrow_mut<$1>(value);
  *p;
  static_assert(::std::is_same<decltype(*p), int&>::value);

  END_LIFETIME($1);

  *p; // compiler error here
}
