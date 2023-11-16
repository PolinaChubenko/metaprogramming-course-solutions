#pragma once

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <string_view>


template<std::size_t max_length>
struct FixedString {
  constexpr FixedString(const char* string, std::size_t length) : length(length) {
    std::copy(string, string + length, storage);
    std::fill(storage + length, storage + max_length, '\0');
  }

  constexpr operator std::string_view() const {
    return std::string_view(storage, length);
  }

  char storage[max_length];
  std::size_t length;
};

constexpr FixedString<256> operator ""_cstr(const char* data, std::size_t size) {
  return FixedString<256>(data, size);
}
