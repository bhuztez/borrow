#include "borrow.hpp"

int
main() {
  using ::borrow::borrow_mut;

  int value = 1;
  BEGIN_LIFETIME($1);

  auto p1 = borrow_mut<$1>(value);

  END_LIFETIME($1);

  auto p2 = borrow_mut<$1>(value); // compiler error here
}
