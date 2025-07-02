#ifndef MY_WEAK_PTR
#define MY_WEAK_PTR

#include "control_block.hpp"
#include "shared_ptr.hpp"

#include <utility>

template <class T>
class SharedPtr;

template <class T>
class WeakPtr {
 public:
  WeakPtr() : control_(nullptr) {
  }

  WeakPtr(const WeakPtr& other) : control_(other.control_) {
    if (control_) {
      ++control_->weak_count;
    }
  }

  WeakPtr(WeakPtr&& other) noexcept : control_(std::exchange(other.control_, nullptr)) {
  }

  WeakPtr(const SharedPtr<T>& shared) : control_(shared.control_) {  // NOLINT
    if (control_) {
      ++control_->weak_count;
    }
  }

  WeakPtr& operator=(const WeakPtr& other) {
    control_ = other.control_;
    if (control_) {
      ++control_->weak_count;
    }

    return *this;
  }

  WeakPtr& operator=(WeakPtr&& other) noexcept {
    control_ = std::exchange(other.control_, nullptr);

    return *this;
  }

  void Swap(WeakPtr<T>& other) noexcept {
    std::swap(control_, other.control_);
  }

  void Reset() {
    if (control_ && --control_->weak_count == 0 && control_->strong_count == 0) {
      delete control_;
    }
    control_ = nullptr;
  }

  int UseCount() const {
    return control_ ? control_->strong_count : 0;
  }

  bool Expired() const {
    if (control_) {
      return control_->strong_count == 0;
    }

    return true;
  }

  SharedPtr<T> Lock() const {
    if (Expired()) {
      return SharedPtr<T>();
    }
    return SharedPtr<T>(*this);
  }

  ~WeakPtr() {
    Reset();
  }

  friend class SharedPtr<T>;

 private:
  IControlBlock* control_;
};

#endif  // MY_WEAK_PTR
