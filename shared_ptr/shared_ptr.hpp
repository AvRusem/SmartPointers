#ifndef MY_SHARED_PTR
#define MY_SHARED_PTR

#include "control_block.hpp"
#include "weak_ptr.hpp"

#include <utility>
#include <stdexcept>
#include <cstddef>
#include <new>

class BadWeakPtr : public std::runtime_error {
 public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {
  }
};

template <class T>
class WeakPtr;

template <class T>
class SharedPtr {
 public:
  SharedPtr() : control_(nullptr) {
  }

  SharedPtr(T* ptr) : control_(ptr ? new ControlBlock<T>(ptr) : nullptr) {  // NOLINT
  }

  SharedPtr(const SharedPtr& other) : control_(other.control_) {
    if (control_) {
      ++control_->strong_count;
    }
  }

  explicit SharedPtr(const WeakPtr<T>& weak) : control_(weak.control_) {
    if (weak.Expired()) {
      throw BadWeakPtr();
    }

    ++control_->strong_count;
  }

  SharedPtr& operator=(const SharedPtr& other) {
    if (this != &other) {
      Reset();
      control_ = other.control_;
      if (control_) {
        ++control_->strong_count;
      }
    }
    return *this;
  }

  SharedPtr(SharedPtr&& other) noexcept : control_(std::exchange(other.control_, nullptr)) {
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
      Reset();
      control_ = std::exchange(other.control_, nullptr);
    }
    return *this;
  }

  void Reset(T* ptr = nullptr) {
    if (control_ && control_->strong_count - 1 == 0) {
      control_->DestroyObject();

      if (control_->weak_count == 0) {
        delete control_;
        control_ = nullptr;
      }
    }

    if (control_) {
      --control_->strong_count;
    }
    control_ = ptr ? new ControlBlock(ptr) : nullptr;
  }

  void Swap(SharedPtr<T>& other) {
    std::swap(control_, other.control_);
  }

  T* Get() const {
    return control_ ? static_cast<T*>(control_->GetObject()) : nullptr;
  }

  int UseCount() const {
    return control_ ? control_->strong_count : 0;
  }

  T& operator*() const {
    return *static_cast<T*>(control_->GetObject());
  }

  T* operator->() const {
    return static_cast<T*>(control_->GetObject());
  }

  explicit operator bool() const {
    return control_ && control_->GetObject();
  }

  ~SharedPtr() {
    Reset();
  }

  friend class WeakPtr<T>;

  template <class U, class... Args>
  friend SharedPtr<U> MakeShared(Args&&... args);

 private:
  IControlBlock* control_;
};

template <class U, class... Args>
SharedPtr<U> MakeShared(Args&&... args) {
  SharedPtr<U> shared_ptr;
  shared_ptr.control_ = new ControlBlockPlacement<U>(std::forward<Args>(args)...);

  return shared_ptr;
}

#endif  // MY_SHARED_PTR
