#pragma once

#include <concepts>

#include "type_tuples.hpp"


namespace type_lists
{

template<class TL>
concept TypeSequence =
    requires {
        typename TL::Head;
        typename TL::Tail;
    };

struct Nil {};

template<class TL>
concept Empty = std::derived_from<TL, Nil>;

template<class TL>
concept TypeList = Empty<TL> || TypeSequence<TL>;

template<class T, TypeList TL>
struct Cons {
    using Head = T;
    using Tail = TL;
};

/*----------- FROM TUPLE -----------*/
namespace detail 
{
template <type_tuples::TypeTuple TT>
struct SFromTuple;

template <class Head, class... Tail>
struct SFromTuple<type_tuples::TTuple<Head, Tail...>> {
    using Type = Cons<Head, typename SFromTuple<type_tuples::TTuple<Tail...>>::Type>;
};

template <>
struct SFromTuple<type_tuples::TTuple<>> {
    using Type = Nil;
};
}

template <type_tuples::TypeTuple TT>
using FromTuple = typename detail::SFromTuple<TT>::Type;


/*----------- TO TUPLE -----------*/
namespace detail 
{
template <TypeList TL, class... Ts>
struct SToTuple {
    using Type = typename SToTuple<typename TL::Tail, Ts..., typename TL::Head>::Type;
};

template <Empty TL, class... Ts>
struct SToTuple<TL, Ts...> {
    using Type = type_tuples::TTuple<Ts...>;
};

}

template <TypeList TL>
using ToTuple = typename detail::SToTuple<TL>::Type;


/*----------- REPEAT -----------*/
template<class T>
struct Repeat {
    using Head = T;
    using Tail = Repeat<T>;
};


/*----------- TAKE -----------*/
namespace detail 
{
template<std::size_t N, TypeList TL>
struct STake {
    using Head = typename TL::Head;
    using Tail = STake<N-1, typename TL::Tail>;
};

template<TypeList TL>
struct STake<0, TL> : Nil {};

template<std::size_t N, Empty TL>
struct STake<N, TL> : Nil {};
}

template<std::size_t N, TypeList TL>
using Take = detail::STake<N, TL>;


/*----------- DROP -----------*/
namespace detail 
{
template<std::size_t N, TypeList TL>
struct SDrop {
    using Type = typename SDrop<N-1, typename TL::Tail>::Type;
};

template<TypeList TL>
struct SDrop<0, TL> {
    using Type = TL;
};

template<std::size_t N, Empty TL>
struct SDrop<N, TL> {
    using Type = TL;
};
}

template<std::size_t N, TypeList TL>
using Drop = typename detail::SDrop<N, TL>::Type;


/*----------- REPLICATE -----------*/
namespace detail 
{
template<std::size_t N, class T>
struct SReplicate {
    using Type = Cons<T, typename SReplicate<N-1, T>::Type>;
};

template<class T>
struct SReplicate<0, T> {
    using Type = Nil;
};
}

template<std::size_t N, class T>
using Replicate = typename detail::SReplicate<N, T>::Type;


/*----------- MAP -----------*/
template<template <typename> class F, TypeList TL>
struct Map {
    using Head = F<typename TL::Head>;
    using Tail = Map<F, typename TL::Tail>;
};

template<template <typename> class F, Empty TL>
struct Map<F, TL> : Nil {};


/*----------- FILTER -----------*/
namespace detail
{
template <template <typename> class P, TypeList TL, bool = false>
struct FilterInner;

template<template <typename> class P, TypeList TL>
struct SFilter;

template<template <typename> class P, TypeSequence TL>
struct SFilter<P, TL> {
    using Type = FilterInner<P, TL, P<typename TL::Head>::Value>;
};

template<template <typename> class P, Empty TL>
struct SFilter<P, TL> {
    using Type = FilterInner<P, TL, false>;
};

template <template <typename> class P, TypeSequence TL>
struct FilterInner<P, TL, true> {
    using Head = typename TL::Head;
    using Tail = typename SFilter<P, typename TL::Tail>::Type;
};

template <template <typename> class P, TypeSequence TL>
struct FilterInner<P, TL, false> : SFilter<P, typename TL::Tail>::Type {
    using typename SFilter<P, typename TL::Tail>::Type::Head;
    using typename SFilter<P, typename TL::Tail>::Type::Tail;
};

template<template <typename> class P, Empty TL, bool b>
struct FilterInner<P, TL, b> : Nil {};

}

template<template <typename> class P, TypeList TL>
using Filter = typename detail::SFilter<P, TL>::Type;


/*----------- ITERATE -----------*/
template<template <typename> class F, typename T>
struct Iterate {
    using Head = T;
    using Tail = Iterate<F, F<T>>;
};


/*----------- CYCLE -----------*/
namespace detail
{
template<TypeList TL, typename... Ts>
struct SCycle {
    using Head = typename TL::Head;
    using Tail = SCycle<typename TL::Tail, Ts..., typename TL::Head>;
};

template<Empty TL, typename T, typename... Ts>
struct SCycle<TL, T, Ts...> {
    using Head = T;
    using Tail = SCycle<TL, Ts..., T>;
};

template<Empty TL>
struct SCycle<TL> : Nil {};

}

template<TypeList TL>
using Cycle = detail::SCycle<TL>;


/*----------- INITS -----------*/
namespace detail {
template<TypeList TL, typename... Ts>
struct SInits {
    using Head = FromTuple<type_tuples::TTuple<Ts...>>;
    using Tail = SInits<typename TL::Tail, Ts..., typename TL::Head>;
};

template<Empty TL, typename... Ts>
struct SInits<TL, Ts...> {
    using Head = FromTuple<type_tuples::TTuple<Ts...>>;
    using Tail = Nil;
};
}

template<TypeList TL>
using Inits = detail::SInits<TL>;


/*----------- TAILS -----------*/
namespace detail {
template<TypeList TL>
struct STails {
    using Head = TL;
    using Tail = STails<typename TL::Tail>;
};

template<Empty TL>
struct STails<TL> {
    using Head = Nil;
    using Tail = Nil;
};
}

template<TypeList TL>
using Tails = detail::STails<TL>;


/*----------- SCANL -----------*/
namespace detail {
template<template <typename, typename> class OP, typename T, TypeList TL>
struct SScanl {
    using Head = T;
    using Tail = SScanl<OP, OP<T, typename TL::Head>, typename TL::Tail>;
};

template<template <typename, typename> class OP, typename T, Empty TL>
struct SScanl<OP, T, TL> {
    using Head = T;
    using Tail = Nil;
};
}

template<template <typename, typename> class OP, typename T, TypeList TL>
using Scanl = detail::SScanl<OP, T, TL>;


/*----------- FOLDL -----------*/
namespace detail {
template<template <typename, typename> class OP, typename T, TypeList TL>
struct SFoldl {
    using Type = OP<typename SFoldl<OP, T, typename TL::Tail>::Type, typename TL::Head>;

};

template<template <typename, typename> class OP, typename T, Empty TL>
struct SFoldl<OP, T, TL> {
    using Type = T;
};
}

template<template <typename, typename> class OP, typename T, TypeList TL>
using Foldl = typename detail::SFoldl<OP, T, TL>::Type;


/*----------- ZIP2 -----------*/
template<TypeList L, TypeList R>
struct Zip2 {
    using Head = type_tuples::TTuple<typename L::Head, typename R::Head>;
    using Tail = Zip2<typename L::Tail, typename R::Tail>;
};

template<TypeList L, TypeList R>
requires Empty<L> || Empty<R>
struct Zip2<L, R> : Nil {};


/*----------- ZIP -----------*/
template<TypeList... TL>
struct Zip {
    using Head = type_tuples::TTuple<typename TL::Head...>;
    using Tail = Zip<typename TL::Tail...>;
};


} // namespace type_lists
