#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>


namespace detail {

  template<auto T>
  constexpr auto Helper() {
    return __PRETTY_FUNCTION__;
  }

  template <class Enum, auto I> requires std::is_enum_v<Enum>
  constexpr Enum GetEnumValue() {
    return static_cast<Enum>(I);
  }

  template <class Enum, long I> requires std::is_enum_v<Enum>
  constexpr bool IsInEnum() {
    const std::string_view str_v = Helper<GetEnumValue<Enum, I>()>();
    return str_v[str_v.rfind('=') + 2] != '(';
  }

  template <class Enum, long I> requires std::is_enum_v<Enum>
  consteval std::string_view GetEnumName() {
    std::string_view str_v = Helper<GetEnumValue<Enum, I>()>();
    std::size_t pos = str_v.rfind(':') + 1;
    if (pos != std::string_view::npos) {
        str_v = str_v.substr(pos, str_v.size() - pos - 1);
    }
    return str_v;
  }

} // namespace

template <class Enum, std::size_t MAXN = 512> requires std::is_enum_v<Enum>
struct EnumeratorTraits {
 private:
  static constexpr auto MAX_LIMIT = static_cast<unsigned long long>(std::numeric_limits<std::underlying_type_t<Enum>>::max());
  static constexpr auto MIN_LIMIT = static_cast<long long>(std::numeric_limits<std::underlying_type_t<Enum>>::min());
  static constexpr int MAX_IT = std::min(MAX_LIMIT, static_cast<unsigned long long>(MAXN));
  static constexpr int MIN_IT = std::max(MIN_LIMIT, -static_cast<long long>(MAXN));

  static consteval std::size_t CalculateEnumSize() {
    std::size_t size = 0;
    if constexpr(std::is_signed<std::underlying_type_t<Enum>>::value && MIN_IT < 0) {
      [&size]<int... I>(std::integer_sequence<int, I...>) {
        ([&size](){
          if (detail::IsInEnum<Enum, I + MIN_IT>()) {
            ++size;
          }
        }(), ...);
      }(std::make_integer_sequence<int, -MIN_IT>());
    }
    [&size]<int... I>(std::integer_sequence<int, I...>) {
      ([&size](){
        if (detail::IsInEnum<Enum, I>()) {
          ++size;
        }
      }(), ...);
    }(std::make_integer_sequence<int, MAX_IT + 1>());
    return size;
  }

  template <auto Func>
  static consteval auto CalculateEnum() {
    std::array<decltype(Func.template operator()<Enum, 0>()), CalculateEnumSize()> result;
    std::size_t i = 0;
    if constexpr(std::is_signed<std::underlying_type_t<Enum>>::value && MIN_IT < 0) {
      [&result, &i]<int... I>(std::integer_sequence<int, I...>) {
        ([&result, &i](){
          if (detail::IsInEnum<Enum, I + MIN_IT>()) {
            result[i] = Func.template operator()<Enum, I + MIN_IT>();
            ++i;
          }
        }(), ...);
      }(std::make_integer_sequence<int, -MIN_IT>());
    }
    [&]<int... I>(std::integer_sequence<int, I...>) {
      ([&result, &i](){
        if (detail::IsInEnum<Enum, I>()) {
          result[i] = Func.template operator()<Enum, I>();
          ++i;
        }
      }(), ...);
    }(std::make_integer_sequence<int, MAX_IT + 1>());
    return result;
  }

 public:
  static constexpr std::size_t size() noexcept {
    return enum_values_.size();
  }

  static constexpr Enum at(std::size_t i) noexcept {
    return enum_values_[i];
  }

  static constexpr std::string_view nameAt(std::size_t i) noexcept {
    return enum_names_[i];
  }

 private:
  static constexpr auto GetNameF_ = []<class Enum_, long I_>() { return detail::GetEnumName<Enum_, I_>(); };
  static constexpr auto GetValueF_ = []<class Enum_, auto I_>() { return detail::GetEnumValue<Enum_, I_>(); };
  static constexpr auto enum_names_ = CalculateEnum<GetNameF_>();
  static constexpr auto enum_values_ = CalculateEnum<GetValueF_>();
};
