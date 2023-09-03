#pragma once


namespace type_tuples
{

template<class... Ts>
struct TTuple {};

template<class TT>
concept TypeTuple = requires(TT t) { []<class... Ts>(TTuple<Ts...>){}(t); };

} // namespace type_tuples
