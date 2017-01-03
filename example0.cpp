#include "borrow.hpp"

int
main() {
  using borrow::ended;

  struct $ {
    struct Lifetime;
  };

  static_assert(not ended(${}, 0));

  struct $::Lifetime {
    struct Ended;
  };

  static_assert(not ended(${}, 0)); // compiler error here
}
