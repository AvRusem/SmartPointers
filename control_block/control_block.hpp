#ifndef MY_CONTROL_BLOCK
#define MY_CONTROL_BLOCK

#include <cstddef>
#include <new>
#include <utility>

struct IControlBlock {
  int strong_count;
  int weak_count;

  IControlBlock() : strong_count(1), weak_count(0) {
  }

  virtual void* GetObject() = 0;
  virtual void DestroyObject() = 0;

  virtual ~IControlBlock() = default;
};

template <class T>
struct ControlBlock : public IControlBlock {
 private:
  T* ptr_;

 public:
  explicit ControlBlock(T* ptr) : IControlBlock(), ptr_(ptr) {
  }

  void* GetObject() override {
    return ptr_;
  }

  void DestroyObject() override {
    delete ptr_;
    ptr_ = nullptr;
  }

  ~ControlBlock() override {
    DestroyObject();
  }
};

template <class T>
struct ControlBlockPlacement : public IControlBlock {
 private:
  alignas(T) std::byte buffer_[sizeof(T)];

 public:
  template <class... Args>
  explicit ControlBlockPlacement(Args&&... args) : IControlBlock() {
    new (buffer_) T(std::forward<Args>(args)...);
  }

  void* GetObject() override {
    return std::launder(reinterpret_cast<T*>(buffer_));
  }

  void DestroyObject() override {
    std::launder(reinterpret_cast<T*>(buffer_))->~T();
  }
};

#endif  // MY_CONTROL_BLOCK