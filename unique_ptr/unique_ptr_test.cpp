#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "unique_ptr.hpp"
#include "unique_ptr.hpp"  // check include guards

#include <memory>
#include <utility>

TEST_CASE("Constructors", "[UniquePtr]") {
  SECTION("Default Constructor") {
    const auto a = UniquePtr<int>();
    REQUIRE_FALSE(a);
    REQUIRE(a.Get() == nullptr);
  }

  SECTION("Pointer Constructor") {
    {
      const auto a = UniquePtr<int>(nullptr);
      REQUIRE_FALSE(a);
      REQUIRE(a.Get() == nullptr);
    }

    {
      const auto ptr = new int;
      const auto a = UniquePtr<int>(ptr);
      REQUIRE(a);
      REQUIRE(a.Get() == ptr);
    }
  }

  SECTION("Move Constructor") {
    static_assert(std::is_nothrow_move_constructible_v<UniquePtr<int>>,
                  "UniquePtr's move constructor must be marked noexcept");
    static_assert(!std::is_copy_constructible_v<UniquePtr<int>>, "UniquePtr must not be copy constructible");

    const auto ptr = new int;
    auto a = UniquePtr<int>(ptr);
    const auto b = std::move(a);
    REQUIRE(b);
    REQUIRE_FALSE(a);  // NOLINT check "moved-from" is valid state
    REQUIRE(a.Get() == nullptr);
    REQUIRE(b.Get() == ptr);
  }
}

TEST_CASE("Assignment", "[UniquePtr]") {
  static_assert(std::is_nothrow_move_assignable_v<UniquePtr<int>>,
                "UniquePtr's move assignment operator must be marked noexcept");
  static_assert(!std::is_copy_assignable_v<UniquePtr<int>>, "UniquePtr must not be copy assignable");

  const auto ptr = new int;
  auto a = UniquePtr<int>(ptr);
  auto b = UniquePtr<int>();

  b = std::move(a);
  REQUIRE(b);
  REQUIRE_FALSE(a);  // NOLINT check "moved-from" is valid state
  REQUIRE(a.Get() == nullptr);
  REQUIRE(b.Get() == ptr);

  const auto ptr2 = new int;
  b = UniquePtr<int>(ptr2);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr2);
}

TEST_CASE("Release", "[UniquePtr]") {
  SECTION("Empty") {
    auto a = UniquePtr<int>();
    REQUIRE(a.Release() == nullptr);
  }

  SECTION("Not Empty") {
    const auto ptr = new int;
    auto a = UniquePtr<int>(ptr);
    REQUIRE(a.Release() == ptr);
    REQUIRE(a.Get() == nullptr);
    REQUIRE_FALSE(a);
    delete ptr;
  }
}

TEST_CASE("Reset", "[UniquePtr]") {
  SECTION("Empty") {
    const auto ptr = new int;
    auto a = UniquePtr<int>();

    a.Reset();
    REQUIRE(!a);
    REQUIRE(a.Get() == nullptr);

    a.Reset(ptr);
    REQUIRE_FALSE(!a);
    REQUIRE(a.Get() == ptr);
  }

  SECTION("Not Empty") {
    const auto ptr1 = new int;
    const auto ptr2 = new int;
    auto a = UniquePtr<int>(ptr1);

    a.Reset(ptr2);
    REQUIRE(a);
    REQUIRE(a.Get() == ptr2);

    a.Reset();
    REQUIRE_FALSE(a);
    REQUIRE(a.Get() == nullptr);
  }
}

TEST_CASE("Swap", "[UniquePtr]") {
  const auto ptr1 = new int;
  const auto ptr2 = new int;
  auto a = UniquePtr<int>();
  auto b = UniquePtr<int>(ptr1);
  auto c = UniquePtr<int>(ptr2);

  a.Swap(a);
  REQUIRE_FALSE(a);
  REQUIRE(a.Get() == nullptr);

  b.Swap(b);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr1);
  REQUIRE_FALSE(b);
  REQUIRE(b.Get() == nullptr);

  b.Swap(c);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr2);
  REQUIRE_FALSE(c);
  REQUIRE(c.Get() == nullptr);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr2);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);
}

TEST_CASE("Operators", "[UniquePtr]") {
  const auto a = UniquePtr(new std::pair<int, double>{});
  REQUIRE(a->first == 0);
  REQUIRE((*a).second == 0.0);

  a->first = 10;
  (*a).second = 11.5;
  REQUIRE(a->first == 10);
  REQUIRE((*a).second == 11.5);

  a.Get()->first = 11;
  a.Get()->second = 0.5;
  REQUIRE(a->first == 11);
  REQUIRE((*a).second == 0.5);
}

TEST_CASE("MakeUnique", "[UniquePtr]") {
  {
    const auto ptr = MakeUnique<std::unique_ptr<int>>();
    REQUIRE(ptr->get() == nullptr);
    static_assert(std::is_same_v<decltype(ptr), const UniquePtr<std::unique_ptr<int>>>,
                  "MakeUnique<std::unique_ptr<int>> must return UniquePtr<std::unique_ptr<int>>");
  }

  {
    const auto ptr = MakeUnique<int>(11);
    REQUIRE(*ptr == 11);
    static_assert(std::is_same_v<decltype(ptr), const UniquePtr<int>>, "MakeUnique<int>() must return UniquePtr<int>");
  }

  {
    const auto ptr = MakeUnique<std::pair<int, double>>(11, 0.5);
    REQUIRE(ptr->first == 11);
    REQUIRE(ptr->second == 0.5);
  }

  {
    int x = 11;
    const auto ptr = MakeUnique<std::pair<int&, std::unique_ptr<int>>>(x, std::make_unique<int>(11));
    REQUIRE(ptr->first == 11);
    REQUIRE(*(ptr->second) == 11);

    x = -11;
    *(ptr->second) = -11;
    REQUIRE(ptr->first == -11);
    REQUIRE(*(ptr->second) == -11);
  }
}
