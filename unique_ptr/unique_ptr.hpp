#ifndef MY_UNIQUE_PTR
#define MY_UNIQUE_PTR

#include <algorithm>
#include <utility>

template <class T>
class UniquePtr {
 public:
  UniquePtr() : data_(nullptr) {
  }

  explicit UniquePtr(T* data) : data_(data) {
  }

  UniquePtr(const UniquePtr& other) = delete;

  UniquePtr(UniquePtr&& other) noexcept : data_(std::exchange(other.data_, nullptr)) {
  }

  UniquePtr& operator=(const UniquePtr& other) = delete;

  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
      delete data_;
      data_ = nullptr;
      Swap(other);
    }

    return *this;
  }

  T* Release() {
    return std::exchange(data_, nullptr);
  }

  void Reset(T* data = nullptr) {
    if (data_) {
      delete data_;
      data_ = nullptr;
    }

    data_ = data;
  }

  void Swap(UniquePtr<T>& other) noexcept {
    std::swap(data_, other.data_);
  }

  T* Get() const {
    return data_;
  }

  T& operator*() const {
    return *data_;
  }

  T* operator->() const {
    return data_;
  }

  explicit operator bool() const {
    return data_ != nullptr;
  }

  ~UniquePtr() {
    delete data_;
  }

 private:
  T* data_ = nullptr;
};

template <class T, class... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

#endif  // MY_UNIQUE_PTR
