======
borrow
======

Constant expressions in C++ are not as constant as you might think. For
example, the following program will cause a compiler error.

.. code:: c++

    // example0.cpp
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

This feature does not seem to be useful at first glance. Thanks to the
brilliant `friend injection techique discovered by Filip Roséen`__ , stateful
metaprogramming has become feasible in C++.

.. __: http://b.atch.se/posts/non-constant-constant-expressions/

Rust, a promising rival to C++, is famed for its borrow checker. With stateful
metaprogramming, we can have the same mechanism implemented at compile-time in
C++.

.. code:: c++

    // example1.cpp
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
