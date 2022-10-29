#pragma once
#include <concepts>


// Allocator is only used in bonus SBO tests,
// ignore if you don't need bonus points.
template <class T /*, class Allocator = std::allocator<T>*/ >
class Spy {
public:
  explicit Spy(T);

  T& operator *();
  const T& operator *() const;

  /* unspecified */ operator ->();

  /*
   * if needed (see task readme):
   *   default constructor
   *   copy and move construction
   *   copy and move assignment
   *   equality operators
   *   destructor
  */

  template <std::invocable<unsigned int> Logger> requires /* see task readme */
  void setLogger(Logger&& logger);

private:
  T value_;
  // Logger logger_; ???
};