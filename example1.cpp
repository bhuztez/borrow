#include <new>
#include <type_traits>
#include <utility>
#include "borrow.hpp"

template<typename T>
struct Container {
  char buf[sizeof(T)];

  void
  write(T&& item) {
    T *ptr = static_cast<T *>(static_cast<void *>(buf));
    new (ptr) T(::std::move(item));
  }

  T
  read() {
    T *ptr = static_cast<T *>(static_cast<void *>(buf));
    T item = ::std::move(*ptr);
    ptr->~T();
    return item;
  }

  T&
  get() {
    T *ptr = static_cast<T *>(static_cast<void *>(buf));
    return *ptr;
  }

  T const&
  get() const {
    T *ptr = static_cast<T *>(static_cast<void *>(buf));
    return *ptr;
  }
};

template<typename T, typename $,
         bool A = ::borrow::available(${}),
         typename = typename ::std::enable_if<A>::type>
auto
get(::borrow::BorrowPtr<Container<T>,$> ptr) {
  return ::borrow::BorrowPtr<T,$> { (*ptr).get() };
}


int
main() {
  auto c = Container<int> {};
  c.write(0);

  BEGIN_LIFETIME($);
  auto p = ::borrow::borrow_mut<$>(c);

  BEGIN_LIFETIME($1);
  auto p1 = ::borrow::borrow_mut<$1>(p);

  auto p2 = get(p1);

  *p2 = 1;

  END_LIFETIME($1);

  *p2; // compiler error here

  END_LIFETIME($);
}
