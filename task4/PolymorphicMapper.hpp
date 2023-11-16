#pragma once

#include <optional>
#include <typeinfo>


template <class From, auto target>
struct Mapping {};

template <class Base, class Target, class... Mappings>
struct PolymorphicMapper {
  static std::optional<Target> map(const Base& object) {
    std::optional<Target> opt;
    (
      [&object, &opt]<typename From, Target target>(Mapping<From, target>) {
        try {
          [[maybe_unused]] const auto& unused = dynamic_cast<const From&>(object);
          if (!opt.has_value()) {
            opt = target;
          }
        } catch (const std::bad_cast&) {}
      }(Mappings{})
      , ...
    );
    return opt;
  }
};
