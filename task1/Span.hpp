#include <span>
#include <concepts>
#include <cstdlib>
#include <iterator>


template
  < class T
  , std::size_t extent = std::dynamic_extent
  >
class Span {
private:

  template
    < std::size_t extent_
    >
  struct DataStorage {
    public:

      constexpr DataStorage() noexcept = default;
      DataStorage(T* data, std::size_t) : data_(data) {}

      constexpr std::size_t Size() const {
        return extent_;
      }

      T* Data() const {
        return data_;
      }

    private:
      T* data_ = nullptr;
  };

  template < >
  struct DataStorage<std::dynamic_extent> {
    public:

      constexpr DataStorage() noexcept = default;
      DataStorage(T* data, std::size_t size) : data_(data), size_(size) {}

      constexpr std::size_t Size() const {
        return size_;
      }

      T* Data() const {
        return data_;
      }

    private:
      T* data_ = nullptr;
      std::size_t size_ = 0;
    
  };

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

  constexpr Span() noexcept = default;

  template
    < class It 
    >
  explicit(extent != std::dynamic_extent)
  constexpr Span(It first, size_type count) : storage_(std::to_address(first), count) {}

  template
    < class It
    , class End 
    >
  explicit(extent != std::dynamic_extent)
  constexpr Span(It first, End last) : storage_(std::to_address(first), last - first) {}

  template
    < std::size_t N 
    >
  constexpr Span(std::type_identity_t<element_type> (&arr)[N]) noexcept : storage_(arr, N) {}

  template
    < class U
    , std::size_t N 
    >
  constexpr Span(std::array<U, N>& arr) noexcept : storage_(arr.data(), N) {}

  template
    < class U
    , std::size_t N 
    >
  constexpr Span(const std::array<U, N>& arr) noexcept : storage_(arr.data(), N) {}


  template
    < class R 
    >
  explicit(extent != std::dynamic_extent)
  constexpr Span(R&& range) : storage_(std::data(range), range.size()) {}

  template
    < class U
    , std::size_t N 
    >
  explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
  constexpr Span(const Span<U, N>& source) noexcept : storage_(source.data(), source.size()) {}

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
    return storage_.Data();
  }

  // Observers

  constexpr size_type Size() const noexcept {
    return storage_.Size();
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
  DataStorage<extent> storage_{};
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
