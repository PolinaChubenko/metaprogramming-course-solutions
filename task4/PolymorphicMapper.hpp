#pragma once

#include <optional>


template <class From, auto target>
struct Mapping;

template <class Base, class Target, class... Mappings>
struct PolymorphicMapper {
  static std::optional<Target> map(const Base& object);
};
