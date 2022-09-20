#include <span>


template <class T, std::size_t extent, std::size_t stride>
class Slice {
public:
  template <class It>
  Slice(It first, std::size_t count, std::size_t skip);

  // ...

  Slice<T, std::dynamic_extent, stride>
    First(std::size_t count);

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    First();

  Slice<T, std::dynamic_extent, stride>
    Last(std::size_t count);

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    Last();

  Slice<T, std::dynamic_extent, stride>
    DropFirst(std::size_t count);

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    DropFirst();
    
  Slice<T, std::dynamic_extent, stride>
    DropLast(std::size_t count);

  template <std::size_t count>
  Slice<T, /*?*/, stride>
    DropLast();

  Slice<T, /*?*/, /*?*/>
    Stride(std::size_t new_stride);
  
  template <std::size_t new_stride>
  Slice<T, /*?*/, /*?*/>
    Stride();

  // ...

private:
  T* data_;
  // std::size_t extent_; ?
  // std::size_t stride_; ?
};
