#pragma once
#include <type_traits>

namespace borrow {
  // borrow lifetime
  struct Root {
  };

  template<typename, typename, typename = void>
  struct Const;

  template<typename Parent, typename Self>
  struct Const<Parent, Self, ::std::void_t<typename Self::Lifetime>>{
  };

  template<typename, typename, typename = void>
  struct Mut;

  template<typename Parent, typename Self>
  struct Mut<Parent, Self, ::std::void_t<typename Self::Lifetime>>{
  };


  template<typename>
  constexpr
  bool
  is_const = false;

  template<typename Parent, typename Self>
  constexpr
  bool
  is_const<Const<Parent,Self>> = true;


  template<typename $, typename = typename $::Lifetime::Ended>
  constexpr
  bool
  ended($, int) {
    return true;
  }

  template<typename $>
  constexpr
  bool
  ended($, ...) {
    return false;
  }


  constexpr
  bool
  all_alive(Root) {
    return true;
  }

  template<typename Parent, typename Self,
           bool E = ended(Self{}, 0),
           bool A = all_alive(Parent{}),
           bool value = ((not E) and A)
           >
  constexpr
  bool
  all_alive(Const<Parent, Self>) {
    return value;
  }

  template<typename Parent, typename Self,
           bool E = ended(Self{}, 0),
           bool A = all_alive(Parent{}),
           bool value = ((not E) and A)
           >
  constexpr
  bool
  all_alive(Mut<Parent, Self>) {
    return value;
  }


  constexpr
  bool
  any_alive(Root) {
    return false;
  }

  template<typename Parent, typename Self,
           bool E = ended(Self{}, 0),
           bool A = any_alive(Parent{}),
           bool value = ((not E) or A)
           >
  constexpr
  bool
  any_alive(Const<Parent, Self>) {
    return value;
  }

  template<typename Parent, typename Self,
           bool E = ended(Self{}, 0),
           bool A = any_alive(Parent{}),
           bool value = ((not E) or A)
           >
  constexpr
  bool
  any_alive(Mut<Parent, Self>) {
    return value;
  }


  // borrow_mut state
  template<typename, int>
  struct Counter{
    friend constexpr auto state(Counter);
  };

  template<typename $, int N, typename T>
  struct Set {
    friend constexpr auto state(Counter<$, N>) {
      return T {};
    }
  };

  template<typename $, int N=0>
  constexpr
  int
  next(float, Counter<$,N> = {}) {
    return N;
  }

  template<typename $, int N=0,
           typename = decltype(state(Counter<$,N>{}))
           >
  constexpr
  int
  next(int, Counter<$,N> = {}, int value = next(0, Counter<$,N+1>{})) {
    return value;
  }


  template<typename Parent, typename Self,
           bool value = all_alive(Const<Parent, Self>{})>
  constexpr
  bool
  available(Const<Parent, Self>) {
    return value;
  }

  template<typename Parent, typename Self,
           bool A = all_alive(Const<Parent, Self>{}),
           typename = typename ::std::enable_if<not A>::type>
  constexpr
  bool
  available(Mut<Parent, Self>) {
    return false;
  }

  template<typename Parent, typename Self,
           bool A = all_alive(Const<Parent, Self>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N == 0)>::type>
  constexpr
  bool
  available(Mut<Parent, Self>) {
    return true;
  }

  template<typename Parent, typename Self,
           bool A = all_alive(Const<Parent, Self>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool A2 = any_alive(State{})>
  constexpr
  bool
  available(Mut<Parent, Self>) {
    return not A2;
  }


  // borrow pointer
  template<typename, typename>
  struct BorrowPtr;

  template<typename T, typename Parent, typename Self>
  struct BorrowPtr<T, Const<Parent, Self>>{
    T const& ref;
  };

  template<typename T, typename Parent, typename Self>
  struct BorrowPtr<T, Mut<Parent, Self>>{
    T& ref;
  };


  template<typename T>
  constexpr
  bool
  is_borrow_ptr = false;

  template<typename T, typename $>
  constexpr
  bool
  is_borrow_ptr<BorrowPtr<T,$>> = true;



  // operator*
  template<typename T, typename Parent, typename Self,
           bool A = all_alive(Const<Parent,Self>{}),
           typename = typename ::std::enable_if<A>::type>
  T const&
  operator*(BorrowPtr<T, Const<Parent,Self>> const& ptr) {
    return ptr.ref;
  }

  template<typename T, typename Parent, typename Self,
           bool A = all_alive(Mut<Parent,Self>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool C = is_const<State>,
           typename = typename ::std::enable_if<C>::type
           >
  T const&
  operator*(BorrowPtr<T, Mut<Parent,Self>> const& ptr) {
    return ptr.ref;
  }

  template<typename T, typename Parent, typename Self,
           bool A = all_alive(Mut<Parent,Self>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N == 0)>::type
           >
  T&
  operator*(BorrowPtr<T, Mut<Parent,Self>>& ptr) {
    return ptr.ref;
  }

  template<typename T, typename Parent, typename Self,
           bool A = all_alive(Mut<Parent,Self>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool A2 = any_alive(State{}),
           typename = typename ::std::enable_if<not A2>::type
           >
  T&
  operator*(BorrowPtr<T, Mut<Parent,Self>>& ptr) {
    return ptr.ref;
  }



  // borrow
  template<typename $, typename T,
           typename = typename ::std::enable_if<not is_borrow_ptr<T>>::type,
           bool A = all_alive(Const<Root,$>{}),
           typename = typename ::std::enable_if<A>::type
           >
  BorrowPtr<T, Const<Root,$>>
  borrow(T const& ref) {
    return {ref};
  }

  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Const<Const<Parent,Self>,$>{}),
           typename = typename ::std::enable_if<A>::type
           >
  BorrowPtr<T, Const<Const<Parent, Self>, $>>
  borrow(BorrowPtr<T, Const<Parent, Self>> const& ptr) {
    return {ptr.ref};
  }


  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Const<Mut<Parent, Self>, $>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N==0)>::type,
           int = sizeof(Set<Self, N, Const<Root, $>>)
           >
  BorrowPtr<T, Const<Mut<Parent, Self>, $>>
  borrow(BorrowPtr<T, Mut<Parent, Self>> const& ptr) {
    return {ptr.ref};
  }

  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Const<Mut<Parent, Self>, $>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool A2 = any_alive(State{}),
           typename = typename ::std::enable_if<not A2>::type,
           int = sizeof(Set<Self, N, Const<Root, $>>)
           >
  BorrowPtr<T, Const<Mut<Parent, Self>, $>>
  borrow(BorrowPtr<T, Mut<Parent, Self>> const& ptr) {
    return {ptr.ref};
  }

  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Const<Mut<Parent, Self>, $>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool A2 = any_alive(State{}),
           typename = typename ::std::enable_if<A2>::type,
           bool C = is_const<State>,
           typename = typename ::std::enable_if<C>::type,
           int = sizeof(Set<Self, N, Const<State, $>>)
           >
  BorrowPtr<T, Const<Mut<Parent, Self>, $>>
  borrow(BorrowPtr<T, Mut<Parent, Self>> const& ptr) {
    return {ptr.ref};
  }



  // borrow_mut
  template<typename $, typename T,
           typename = typename ::std::enable_if<not is_borrow_ptr<T>>::type,
           bool A = all_alive(Mut<Root,$>{}),
           typename = typename ::std::enable_if<A>::type
           >
  BorrowPtr<T, Mut<Root,$>>
  borrow_mut(T& ref) {
    return {ref};
  }


  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Mut<Mut<Parent, Self>, $>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N==0)>::type,
           int = sizeof(Set<Self, N, Mut<Root, $>>)
           >
  BorrowPtr<T, Mut<Mut<Parent, Self>, $>>
  borrow_mut(BorrowPtr<T, Mut<Parent, Self>>& ptr) {
    return {ptr.ref};
  }


  template<typename $, typename T, typename Parent, typename Self,
           bool A = all_alive(Mut<Mut<Parent, Self>, $>{}),
           typename = typename ::std::enable_if<A>::type,
           int N = next<Self>(0),
           typename = typename ::std::enable_if<(N > 0)>::type,
           typename State = decltype(state(Counter<Self,N-1>{})),
           bool A2 = any_alive(State{}),
           typename = typename ::std::enable_if<not A2>::type,
           int = sizeof(Set<Self, N, Mut<Root, $>>)
           >
  BorrowPtr<T, Mut<Mut<Parent, Self>, $>>
  borrow_mut(BorrowPtr<T, Mut<Parent, Self>>& ptr) {
    return {ptr.ref};
  }

}

#define BEGIN_LIFETIME(s) struct s { struct Lifetime; };
#define END_LIFETIME(s) struct s::Lifetime { struct Ended; };
