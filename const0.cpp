#include "borrow.hpp"

int
main() {
  using ::borrow::borrow;

  int value = 1;
  struct $1 {
  };

  auto p1 = borrow<$1>(value); // compiler error here
}
