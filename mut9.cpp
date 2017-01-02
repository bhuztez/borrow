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

  auto p1 = borrow_mut<$1>(value);
  *p1;
  static_assert(::std::is_same<decltype(*p1), int&>::value);

  auto p2 = borrow<$2>(p1);
  *p1;
  static_assert(::std::is_same<decltype(*p1), int const&>::value);
  END_LIFETIME($2);

  *p1;
  static_assert(::std::is_same<decltype(*p1), int&>::value);

  auto p3 = borrow_mut<$3>(p1);
  END_LIFETIME($3);
  *p1;
  static_assert(::std::is_same<decltype(*p1), int&>::value);

  auto p4 = borrow_mut<$4>(p1);
  *p1; // compiler error here

  END_LIFETIME($1);
  END_LIFETIME($4);
}
