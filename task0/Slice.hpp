#include <span>
#include <concepts>
#include <cstdlib>
#include <array>
#include <iterator>


template 
  < class T
  , std::size_t extent = std::dynamic_extent
  , std::size_t stride = 1
  >
class Slice {
public:
  template<class U>
  Slice(U& container);

  template <std::contiguous_iterator It>
  Slice(It first, std::size_t count, std::size_t skip);

  // Data, Size, Stride, begin, end, casts, etc...

  Slice<T, std::dynamic_extent, stride>
    First(std::size_t count) const;

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    First() const;

  Slice<T, std::dynamic_extent, stride>
    Last(std::size_t count) const;

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    Last() const;

  Slice<T, std::dynamic_extent, stride>
    DropFirst(std::size_t count) const;

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    DropFirst() const;
    
  Slice<T, std::dynamic_extent, stride>
    DropLast(std::size_t count) const;

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    DropLast() const;

  Slice<T, /*?*/, /*?*/>
    Skip(std::size_t skip) const;
  
  template <std::size_t skip>
  Slice<T, /*?*/, /*?*/>
    Skip() const;

private:
  T* data_;
  // std::size_t extent_; ?
  // std::size_t stride_; ?
};
