#pragma once


template<size_t max_length>
struct FixedString {
  FixedString(const char* string, size_t length);
  operator std::string_view() const;

  // std::string impl; ???
};

// operator ""_cstr ?
