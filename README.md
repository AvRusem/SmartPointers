# SmartPointer

## ENGLISH

This repository contains educational implementations of smart pointers in C++, including:

- `UniquePtr<T>` – exclusive ownership smart pointer (non-copyable, movable).
- `SharedPtr<T>` – reference-counted smart pointer with shared ownership.
- `WeakPtr<T>` – non-owning weak reference to a `SharedPtr<T>`.

### Features

- Custom control block implementation for `SharedPtr` and `WeakPtr`.
- Support for `MakeShared` for efficient memory allocation.
- Manual memory management using `placement new`.
- No use of atomic operations — **not thread-safe**.
- Designed for learning and demonstration purposes.
- **Array support is not implemented** (`T[]` is not supported).

### Example

```cpp
UniquePtr<int> ptr(new int(5));
SharedPtr<std::string> str = MakeShared<std::string>("Hello, world");
