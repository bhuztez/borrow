#include "borrow.hpp"

int
main() {
  using ::borrow::borrow;

  int value = 1;
  BEGIN_LIFETIME($1);

  auto p1 = borrow<$1>(value);

  END_LIFETIME($1);

  auto p2 = borrow<$1>(value); // compiler error here
}
