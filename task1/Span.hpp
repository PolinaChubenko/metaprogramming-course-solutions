#include <span>
#include <concepts>
#include <cstdlib>
#include <iterator>


template
  < class T
  , std::size_t extent = std::dynamic_extent
  >
class Span {
public:
  // Reimplement the standard span interface here
  // (some more exotic methods are not checked by the tests and can be sipped)
  // Note that unliike std, the methods name should be Capitalized!
  // E.g. instead of subspan, do Subspan.
  // Note that this does not apply to iterator methods like begin/end/etc.

private:
  T* data_;
  // std::size_t extent_; ?
};
