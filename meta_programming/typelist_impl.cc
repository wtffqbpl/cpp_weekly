#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace type_list_ns {

template <typename... Elements> class Typelist {};

// using SignedIntegralTypes = Typelist<signed char, short, int, long, long
// long>;

template <typename List> class FrontT;

template <typename Head, typename... Tail>
class FrontT<Typelist<Head, Tail...>> {
public:
  using Type = Head;
};

template <typename List> using Front = typename FrontT<List>::Type;

// Front<SignedIntegralTypes> ---> signed char

// PopFront meta-function removes the first element from the typelist.
template <typename List> class PopFrontT;

template <typename Head, typename... Tail>
class PopFrontT<Typelist<Head, Tail...>> {
public:
  using Type = Typelist<Tail...>;
};

template <typename List> using PopFront = typename PopFrontT<List>::type;

// PopFront<SignedIntegralTypes> ---> Typelist<short, int, long, long long>

// PushFront --- Insert elements onto the front of the typelist by capturing
// all of the existing elements into a template parameter pack, then creating
// a new Typelist specialization containing all of those elements.

template <typename List, typename NewElement> class PushFrontT;

template <typename... Elements, typename NewElement>
class PushFrontT<Typelist<Elements...>, NewElement> {
public:
  using Type = Typelist<NewElement, Elements...>;
};

template <typename List, typename NewElement>
using PushFront = typename PushFrontT<List, NewElement>::Type;

// PushFront<SignedIntegralTypes, bool> ---> Typelist<bool, signed char, short,
// int, long, long long>;

/// INDEXING
template <typename List, unsigned N>
class NthElementT : public NthElementT<PopFrontT<List>, N - 1> {};

// basis case:
template <typename List> class NthElementT<List, 0> : public FrontT<List> {};

template <typename List, unsigned N>
using NthElement = typename NthElementT<List, N>::Type;

/// FINDING THE BEST MATCH
// Many typelist algorithms search for data within the typelist. For example,
// one may want to find the largest type within the typelist (e.g. to allocate
// enough storage for any of the types in the list). This too can be
// accomplished with a recursive template metaprogram:

template <typename List> class IsEmpty {
public:
  static constexpr bool value = false;
};

template <> class IsEmpty<Typelist<>> {
public:
  static constexpr bool value = true;
};

template <typename List> auto is_empty_typelist_v = IsEmpty<List>::value;

template <typename List, bool Empty = IsEmpty<List>::value> class LargestTypeT;

// recursive case:
template <typename List> class LargestTypeT<List, false> {
private:
  using Contender = FrontT<List>;
  using Rest = typename LargestTypeT<PopFrontT<List>>::Type;

public:
  using Type =
      std::conditional_t<(sizeof(Contender) >= sizeof(Rest)), Contender, Rest>;
};

// basis case:
template <typename List> class LargestTypeT<List, true> {
public:
  using Type = char;
};

// The LargestType algorithm will return the first, largest type within the
// typelist.
template <typename List> using LargestType = typename LargestTypeT<List>::Type;

// The defaulted second template parameter to LargestTypeT, Empty, checkes
// whether the list is empty. If not, the recursive case (which fixes this
// argument to false) continues exploring the list. Otherwise, the basis case
// (which fixes this argument to true) terminates the recursion and provides the
// initial result (char).

/// APPENDING TO A TYPELIST

template <typename List, typename NewElement, bool = IsEmpty<List>::value>
class PushBackRecT;

// recursive case:
template <typename List, typename NewElement>
class PushBackRecT<List, NewElement, false> {
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using NewTail = typename PushBackRecT<Tail, NewElement>::Type;

public:
  using Type = PushFront<Head, NewTail>;
};

// basis case:
template <typename List, typename NewElement>
class PushBackRecT<List, NewElement, true> {
public:
  using Type = PushFront<List, NewElement>;
};

// generic push-back operation:
template <typename List, typename NewElement>
class PushBackT : public PushBackRecT<List, NewElement> {};

template <typename List, typename NewElement>
using PushBack = typename PushBackT<List, NewElement>::Type;

/// REVERSING A TYPELIST

template <typename List, bool Empty = IsEmpty<List>::value> class ReverseT;

template <typename List> using Reverse = typename ReverseT<List>::Type;

// recursive case:
template <typename List>
class ReverseT<List, false>
    : public PushBackT<Reverse<PopFront<List>>, Front<List>> {};

// basis case:
template <typename List> class ReverseT<List, true> {
public:
  using Type = List;
};

// POP BACK
template <typename List> class PopBackT {
public:
  using Type = Reverse<PopFront<Reverse<List>>>;
};

template <typename List> using PopBack = typename PopBackT<List>::Type;

/// TRANSFORMING A TYPELIST
// "transform" all the types in the typelist in some way. Such as by turning
// each type into its const-qualified variant using the AddConst meta-function.
template <typename T> struct AddConstT { using Type = T const; };

template <typename T> using AddConst = typename AddConstT<T>::Type;

template <typename List, template <typename T> class MetaFun,
          bool Empty = IsEmpty<List>::value>
class TransformT;

// recursive case:
template <typename List, template <typename T> class MetaFun>
class TransformT<List, MetaFun, false>
    : public PushFrontT<typename TransformT<PopFrontT<List>, MetaFun>::Type,
                        typename MetaFun<Front<List>>::Type> {};

// basis case:
template <typename List, template <typename T> class MetaFun>
class TransformT<List, MetaFun, true> {
public:
  using Type = List;
};

// Transform is a useful algorithm for transforming each of the elements of the
// sequence.
template <typename List, template <typename T> class MetaFun>
using Transform = typename TransformT<List, MetaFun>::Type;

} // namespace type_list_ns
