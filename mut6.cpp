#include "borrow.hpp"

int
main() {
  using ::borrow::borrow_mut;

  int value = 1;
  BEGIN_LIFETIME($1);
  BEGIN_LIFETIME($2);
  BEGIN_LIFETIME($3);
  BEGIN_LIFETIME($4);

  auto p1 = borrow_mut<$1>(value);
  auto p2 = borrow_mut<$2>(p1);
  END_LIFETIME($2);

  auto p3 = borrow_mut<$3>(p1);
  auto p4 = borrow_mut<$4>(p1); // compiler error here

  END_LIFETIME($1);
  END_LIFETIME($3);
  END_LIFETIME($4);
}
