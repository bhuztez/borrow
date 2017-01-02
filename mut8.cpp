#include "borrow.hpp"

int
main() {
  using ::borrow::borrow;
  using ::borrow::borrow_mut;

  int value = 1;
  BEGIN_LIFETIME($1);
  BEGIN_LIFETIME($2);
  BEGIN_LIFETIME($3);
  BEGIN_LIFETIME($4);
  BEGIN_LIFETIME($5);
  BEGIN_LIFETIME($6);
  BEGIN_LIFETIME($7);

  auto p1 = borrow_mut<$1>(value);
  auto p2 = borrow<$2>(p1);
  auto p3 = borrow<$3>(p1);
  END_LIFETIME($2);
  END_LIFETIME($3);

  auto p4 = borrow_mut<$4>(p1);
  END_LIFETIME($4);

  auto p5 = borrow<$5>(p1);
  auto p6 = borrow<$6>(p1);
  auto p7 = borrow_mut<$7>(p1); // compiler error here

  END_LIFETIME($1);
  END_LIFETIME($5);
  END_LIFETIME($6);
  END_LIFETIME($7);
}
