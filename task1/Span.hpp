#include <span>
#include <concepts>
#include <cstdlib>
#include <iterator>
#include <cassert>


namespace detail {
  template
    < std::size_t extent
    >
  struct SpanBase {
    constexpr SpanBase() noexcept = default;
    SpanBase(std::size_t e) {
      assert(e == extent);
    }

    constexpr std::size_t Extent() const {
      return extent;
    }
  };

  template < >
  struct SpanBase<std::dynamic_extent> {
   public:

    constexpr SpanBase() noexcept = default;
    SpanBase(std::size_t extent) : extent_(extent) {}

    [[gnu::always_inline]] std::size_t Extent() const {
      return extent_;
    }

   private:
    std::size_t extent_ = 0;
  };
}



template
  < class T
  , std::size_t extent = std::dynamic_extent
  >
class Span : detail::SpanBase<extent> {
 private:

  using Base = detail::SpanBase<extent>;
  using Base::Extent;

 public:

  // Named requirements

  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type =	std::size_t;
  using difference_type	= std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

  // Constructors

  constexpr Span() noexcept requires (extent == 0 || extent == std::dynamic_extent) = default;

  template
    < std::contiguous_iterator It 
    >
  explicit(extent != std::dynamic_extent)
  constexpr Span(It first, size_type count) : Base(count), data_(std::to_address(first)) {}

  template
    < std::contiguous_iterator It
    , std::contiguous_iterator End 
    >
    requires (std::same_as<typename It::value, typename End::value>)
  explicit(extent != std::dynamic_extent)
  constexpr Span(It first, End last) : Base(last - first), data_(std::to_address(first)) {}

  template
    < std::size_t N 
    >
  constexpr Span(std::type_identity_t<element_type> (&arr)[N]) noexcept : Base(N), data_(arr) {}

  template
    < class U
    , std::size_t N 
    >
  constexpr Span(std::array<U, N>& arr) noexcept : Base(N), data_(arr.data()) {}

  template
    < class U
    , std::size_t N 
    >
  constexpr Span(const std::array<U, N>& arr) noexcept : Base(N), data_(arr.data()) {}


  template
    < class R 
    >
  explicit(extent != std::dynamic_extent)
  constexpr Span(R&& range) : Base(range.size()), data_(std::data(range)) {}

  template
    < class U
    , std::size_t N 
    >
  explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
  constexpr Span(const Span<U, N>& source) noexcept : Base(source.size()), data_(source.data()) {}

  constexpr Span(const Span& other) noexcept = default;

  ~Span() noexcept = default;

  Span& operator=(const Span& other) noexcept = default;

  // Element access methods

  constexpr reference Front() const {
    return *Data();
  }

  constexpr reference Back() const {
    return *(Data() + (Size() - 1));
  }

  constexpr reference operator[](size_type idx) const {
    return *(Data() + idx);
  }

  constexpr pointer Data() const noexcept {
    return data_;
  }

  // Observers

  constexpr size_type Size() const noexcept {
    return Extent();
  }

  constexpr size_type Size_bytes() const noexcept {
    return Size() * sizeof(element_type);
  }

  [[nodiscard]] constexpr bool Empty() const noexcept {
    return Size() == 0;
  }

  // Subviews

  template
    < std::size_t Count 
    >
  constexpr Span<element_type, Count> First() const {
    return Span<element_type, Count>{Data(), Count};
  }

  constexpr Span<element_type, std::dynamic_extent> First(size_type Count) const {
    return {Data(), Count};
  }

  template
    < std::size_t Count 
    >
  constexpr Span<element_type, Count> Last() const {
     return Span<element_type, Count>{Data() + (Size() - Count), Count};
  }

  constexpr Span<element_type, std::dynamic_extent> Last(size_type Count) const {
    return {Data() + (Size() - Count), Count};
  }

  // Iterators

  constexpr iterator begin() const noexcept { 
    return Data(); 
  }

  constexpr iterator end() const noexcept { 
    return Data() + Size(); 
  }

  constexpr reverse_iterator rbegin() const noexcept {
      return reverse_iterator(end());
  }

  constexpr reverse_iterator rend() const noexcept {
      return reverse_iterator(begin());
  }


private:
  T* data_ = nullptr;
};


// Deduction guides

template
    < class It
    , class EndOrSize
    >
Span(It, EndOrSize) -> Span<std::remove_reference_t<std::iter_reference_t<It>>>;

template
    < class R 
    >
Span(R&&) -> Span<std::remove_reference_t<std::ranges::range_reference_t<R>>>;

template
  < class T
  , std::size_t N 
  >
Span(std::array<T, N>&) -> Span<T, N>;

template
  < class T
  , std::size_t N 
  >
Span(const std::array<T, N>&) -> Span<const T, N>;
