#pragma once
#include <compare>
#include <concepts>
#include <memory>
#include <type_traits>


namespace detail {
  struct ILoggerHolder {
    virtual ILoggerHolder* MakeCopy() = 0;
    virtual void Log(unsigned int) = 0;
    virtual ~ILoggerHolder() = default;
  };

  template <typename Logger>
  struct LoggerHolder : ILoggerHolder {
    Logger logger;

    template <typename F>
    LoggerHolder(F&& f) : logger(std::forward<F>(f)) {}

    ILoggerHolder* MakeCopy() override {
      if constexpr(std::is_copy_constructible_v<Logger>) {
        return new LoggerHolder(logger);
      }
      return nullptr;
    }

    void Log(unsigned int cnt) override {
      logger(cnt);
    }

    ~LoggerHolder() override = default;
  };
}

template <class T>
class Spy {
  class SpyWrapper {
    Spy& spy_;
   public:
    SpyWrapper(Spy& spy) : spy_(spy) {}

    T* operator->() {
      return &spy_.value_;
    }

    ~SpyWrapper() {
      if (--spy_.wrapper_cnt_ == 0) {
        if (spy_.logger_) {
          spy_.logger_->Log(spy_.calls_cnt_);
        }
        spy_.calls_cnt_ = 0;
      }
    }
  };
public:
  Spy() = default;

  explicit Spy(T value) : value_(std::move(value)) {}

  Spy(const Spy& other) requires(std::copy_constructible<T>) 
  : value_(other.value_) {
    if (other.logger_) {
      logger_.reset(other.logger_->MakeCopy());
    }
  }

  Spy& operator=(const Spy& other) requires(std::copyable<T>) {
    Spy temp = other;
    std::swap(value_, temp.value_);
    std::swap(logger_, temp.logger_);
    return *this;
  }

  Spy(Spy&& other) requires(std::move_constructible<T>) 
  : value_(std::move(other.value_)), logger_(std::move(other.logger_)) {}

  Spy& operator=(Spy&& other) requires (std::movable<T>) {
    Spy temp = std::move(other);
    std::swap(value_, temp.value_);
    std::swap(logger_, temp.logger_);
    return *this;
  }

  auto operator<=>(const Spy& other) const requires (std::three_way_comparable<T>) {
    return value_ <=> other.value_;
  }

  bool operator==(const Spy& other) const requires (std::equality_comparable<T>) {
    return value_ == other.value_;
  }

  T& operator*() {
    return value_;
  }

  const T& operator*() const {
    return value_;
  }

  SpyWrapper operator->() {
    ++calls_cnt_;
    ++wrapper_cnt_;
    return SpyWrapper(*this);
  }

  // Resets logger
  void setLogger() {
    logger_.reset();
  }

  // T copy -> Logger copy == !T copy V Logger copy
  template <std::invocable<unsigned int> Logger> requires (
    (!std::copy_constructible<T> || std::copy_constructible<std::remove_reference_t<Logger>>) &&
    (!std::move_constructible<T> || std::move_constructible<std::remove_reference_t<Logger>>)
  )
  void setLogger(Logger&& logger) {
    logger_.reset(new detail::LoggerHolder<std::remove_reference_t<Logger>>(std::forward<Logger>(logger)));
  }

private:
  T value_;
  std::unique_ptr<detail::ILoggerHolder> logger_;
  unsigned int calls_cnt_ = 0;
  unsigned int wrapper_cnt_ = 0;
};
