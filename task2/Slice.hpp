#include <span>
#include <concepts>
#include <cstdlib>
#include <array>
#include <iterator>
#include <cassert>


inline constexpr std::ptrdiff_t dynamic_stride = -1;

namespace detail {
  template
    < std::size_t extent_
    >
  struct ExtentImpl {
    constexpr ExtentImpl() noexcept = default;
    constexpr ExtentImpl(std::size_t e) {
      assert(e == extent_);
    }
    constexpr std::size_t Extent() const {
      return extent_;
    }
  };

  template < >
  struct ExtentImpl<std::dynamic_extent> {
    constexpr ExtentImpl() noexcept = default;
    constexpr ExtentImpl(std::size_t e) : extent_(e) {}
    [[gnu::always_inline]] std::size_t Extent() const {
      return extent_;
    }
   private:
    std::size_t extent_ = 0;
  };

  template
    < std::ptrdiff_t stride_
    >
  struct StrideImpl {
    constexpr StrideImpl() noexcept = default;
    constexpr StrideImpl(std::ptrdiff_t s) {}
    constexpr std::ptrdiff_t Stride() const {
      return stride_;
    }
  };

  template < >
  struct StrideImpl<dynamic_stride> {
    constexpr StrideImpl() noexcept = default;
    constexpr StrideImpl(std::ptrdiff_t s) : stride_(s) {}
    [[gnu::always_inline]] std::ptrdiff_t Stride() const {
      return stride_;
    }
   private:
    std::ptrdiff_t stride_ = -1;
  };

  constexpr std::size_t CeilDiv(std::size_t a, std::ptrdiff_t b) {
    return (a + b - 1) / b;
  }
}


template
  < class T
  , std::size_t extent = std::dynamic_extent
  , std::ptrdiff_t stride = 1
  >
class Slice {
 private:
  template
    < std::size_t extent_
    , std::ptrdiff_t stride_
    >
  struct DataStorage : private detail::ExtentImpl<extent_>, private detail::StrideImpl<stride_> {
   private: 
    using TExtent = detail::ExtentImpl<extent_>;
    using TStride = detail::StrideImpl<stride_>;
   public:
    constexpr DataStorage() noexcept = default;
    constexpr DataStorage(T* data, std::size_t e, std::ptrdiff_t s) : TExtent(e), TStride(s), data_(data) {}

    constexpr std::size_t Extent() const {
      return TExtent::Extent();
    }

    constexpr std::ptrdiff_t Stride() const {
      return TStride::Stride();
    }

    T* Data() const {
      return data_;
    }

   private:
    T* data_ = nullptr;
  };

 public:

  // Iterator 

  struct Iterator : detail::StrideImpl<stride> {
  private:
    using TStride = detail::StrideImpl<stride>;
    using TStride::Stride;
  public:

    // Traits
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

  private:
    pointer ptr_;

  public:

    // Constructors

    Iterator() = default;

    Iterator(pointer ptr, difference_type skip) : TStride(skip), ptr_(ptr) {}

    // Access

    reference operator*() const {
      return *ptr_;
    }

    pointer operator->() const {
      return ptr_;
    }

    reference operator[](difference_type n) const {
      return ptr_ + n * Stride();
    }

    // Bidirectional math

    Iterator& operator++() {
      ptr_ += Stride();
      return *this;
    }

    Iterator operator++(int) {
      Iterator copy = *this;
      ptr_ += Stride();
      return copy;
    }

    Iterator& operator--() {
      ptr_ -= Stride();
      return *this;
    }

    Iterator operator--(int) {
      Iterator copy = *this;
      ptr_ -= Stride();
      return copy;
    }

    // Math

    Iterator& operator+=(int offset) {
      ptr_ += offset * Stride();
      return *this;
    }

    Iterator& operator-=(int offset) {
      ptr_ -= offset * Stride();
      return *this;
    }

    Iterator operator+(difference_type offset) const {
      Iterator copy = *this;
      copy += offset;
      return copy;
    }

    friend Iterator operator+(difference_type offset, const Iterator& it) {
      Iterator copy = it;
      copy += offset;
      return copy;
    }

    Iterator operator-(difference_type offset) const {
      Iterator copy = *this;
      copy -= offset;
      return copy;
    }

    difference_type operator-(const Iterator& other) const {
      return (ptr_ - other.ptr_) / Stride();
    }

    // Comparison

    bool operator<=>(const Iterator& other) const {
      return ptr_ <=> other.ptr_;
    }

    bool operator==(const Iterator& other) const {
      return ptr_ == other.ptr_;
    }
  };

  // Named requirements

  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type =	std::size_t;
  using difference_type	= std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = Iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;

  // Constructors

  constexpr Slice() noexcept = default;

  template
    < class U
    >
  constexpr Slice(U& container) : storage_(std::data(container), container.size(), 0) {}

  template 
    < std::contiguous_iterator It
    >
  constexpr Slice(It first, std::size_t count, std::ptrdiff_t skip) : storage_(std::to_address(first), count, skip) {}

  explicit constexpr Slice(const Slice<T, extent, stride>& other) noexcept = default;

  template
    < class U
    , std::size_t e
    , std::ptrdiff_t s
    >
  constexpr Slice(Slice<U, e, s>& other) noexcept : storage_(other.Data(), other.Size(), other.Stride()) {}

  template
    < class U
    , std::size_t e
    , std::ptrdiff_t s
    >
  constexpr Slice(const Slice<U, e, s>& other) noexcept : storage_(other.Data(), other.Size(), other.Stride()) {}

  ~Slice() noexcept = default;

  // Observers

  constexpr size_type Size() const noexcept {
    return storage_.Extent();
  }

  constexpr std::ptrdiff_t Stride() const noexcept {
    return storage_.Stride();
  }

  [[nodiscard]] constexpr bool Empty() const noexcept {
    return Size() == 0;
  }

  // Element access methods

  constexpr pointer Data() const noexcept {
    return storage_.Data();
  }

  constexpr reference operator[](size_type idx) const {
    return *(Data() + (Stride() * idx));
  }

  // Subviews

  Slice <element_type, std::dynamic_extent, stride> First(std::size_t count) const {
    return {Data(), count, stride};
  }

  template
    < std::size_t count
    >
  Slice <element_type, count, stride> First() const {
    return {Data(), count, stride};
  }

  Slice <element_type, std::dynamic_extent, stride> Last(std::size_t count) const {
    return {Data() + (Size() - count) * Stride(), count, Stride()};
  }

  template 
    < std::size_t count
    >
  Slice <element_type, count, stride> Last() const {
    return {Data() + (Size() - count) * Stride(), count, Stride()};
  }

  Slice <element_type, std::dynamic_extent, stride> DropFirst(std::size_t count) const {
    return {Data() + count * Stride(), Size() - count, Stride()};
  }

  template 
    < std::size_t count
    >
  Slice<element_type, std::dynamic_extent, stride> DropFirst() const {
    return {Data() + count * Stride(), Size() - count, Stride()};
  }

  Slice <element_type, std::dynamic_extent, stride> DropLast(std::size_t count) const {
    return {Data(), Size() - count, Stride()};
  }

  template 
    < std::size_t count
    >
  Slice <element_type, std::dynamic_extent, stride> DropLast() const {
    return {Data(), Size() - count, Stride()};
  }

  constexpr Slice <element_type, std::dynamic_extent, dynamic_stride> Skip(std::ptrdiff_t skip) const {
    auto new_stride = Stride() * skip;
    return {Data(), detail::CeilDiv(Size(), skip), new_stride};
  }

  template 
    < std::ptrdiff_t skip
    >
  constexpr auto Skip() const {
    if constexpr (stride != dynamic_stride) {
      constexpr auto new_stride = stride * skip;
      if constexpr (extent != std::dynamic_extent) {
        constexpr auto new_extent = detail::CeilDiv(extent, skip);
        return Slice<element_type, new_extent, new_stride>(Data(), new_extent, new_stride);
      } else {
        auto new_extent = detail::CeilDiv(Size(), skip);
        return Slice<element_type, std::dynamic_extent, new_stride>(Data(), new_extent, new_stride);
      }
    } else {
      auto new_stride = Stride() * skip;
      if constexpr (extent != std::dynamic_extent) {
        constexpr auto new_extent = detail::CeilDiv(extent, skip);
        return Slice<element_type, new_extent, dynamic_stride>(Data(), new_extent, new_stride);
      } else {
        auto new_extent = detail::CeilDiv(Size(), skip);
        return Slice<element_type, std::dynamic_extent, dynamic_stride>(Data(), new_extent, new_stride);
      }
    }
  }
  
  // Iterators

  constexpr iterator begin() const noexcept { 
    return iterator(Data(), Stride()); 
  }

  constexpr iterator end() const noexcept { 
    return iterator(Data() + (Size() * Stride()), Stride()); 
  }

  constexpr reverse_iterator rbegin() const noexcept {
      return reverse_iterator(end());
  }

  constexpr reverse_iterator rend() const noexcept {
      return reverse_iterator(begin());
  }

private:
  DataStorage<extent, stride> storage_{};
};


// Comparison of Slice

template
  < class T1
  , std::size_t e1
  , std::ptrdiff_t s1
  , class T2
  , std::size_t e2
  , std::ptrdiff_t s2
  >
constexpr bool operator==(const Slice<T1, e1, s1>& sl1, const Slice<T2, e2, s2>& sl2) noexcept {
  return sl1.Size() == sl2.Size() && sl1.Stride() == sl2.Stride() && sl1.Data() == sl2.Data();
}


// Deduction guides


template
  < std::contiguous_iterator It
  >
Slice(It, std::size_t, std::ptrdiff_t) -> Slice<std::remove_reference_t<std::iter_reference_t<It>>, std::dynamic_extent, dynamic_stride>;

template
  < class T
  , std::size_t N 
  >
Slice(std::array<T, N>&) -> Slice<T, N>;

template 
  < class U
  >
Slice(U&) -> Slice<typename U::value_type>;

template 
  < class U
  >
Slice(const U&) -> Slice<const typename U::value_type>;


