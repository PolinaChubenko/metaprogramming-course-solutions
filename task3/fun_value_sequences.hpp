#pragma once

#include <value_types.hpp>
#include <type_lists.hpp>
#include <utility>


/*----------- NATS -----------*/
namespace detail 
{
using namespace value_types;
template<typename tag>
using PlusOne = ValueTag<tag::Value + 1>;
}
using Nats = type_lists::Iterate<detail::PlusOne, value_types::ValueTag<0>>;


/*----------- FIB -----------*/
namespace detail {
template <typename tag>
using FibNext = ValueTag<std::make_pair(tag::Value.first + tag::Value.second, tag::Value.first)>;

template <typename tag>
using DropSecond = ValueTag<tag::Value.first>;
}

using Fib = type_lists::Map<detail::DropSecond, type_lists::Iterate<detail::FibNext, value_types::ValueTag<std::make_pair(0, 1)>>>;


/*----------- PRIMES -----------*/
namespace detail {

consteval bool IsPrimeF(int a) {
    auto limit = a/2 + 1;
    for(int i = 2; i < limit; ++i) {
        if (a % i == 0) {
            return false;
        }
    }
    return true;
}
template <typename tag>
struct IsPrime {
    static constexpr bool Value = IsPrimeF(tag::Value);
};

template <>
struct IsPrime<ValueTag<0>> {
    static constexpr bool Value = false;
};

template <>
struct IsPrime<ValueTag<1>> {
    static constexpr bool Value = false;
};
}

using Primes = type_lists::Filter<detail::IsPrime, Nats>;
