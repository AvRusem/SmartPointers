#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "shared_ptr.hpp"
#include "weak_ptr.hpp"
#include "weak_ptr.hpp"  // check include guards

#include <utility>

TEST_CASE("Weak Constructors", "[WeakPtr]") {
  const auto shared = SharedPtr(new int);
  const auto a = WeakPtr<int>();
  auto b = a;
  [[maybe_unused]] const auto c = WeakPtr(shared);
  [[maybe_unused]] const auto d = std::move(b);
}

TEST_CASE("UseCountAndExpired", "[WeakPtr]") {
  {
    const auto a = WeakPtr<int>();
    auto b = a;
    const auto c = std::move(b);

    REQUIRE(a.UseCount() == 0);
    REQUIRE(b.UseCount() == 0);  // NOLINT check "moved-from" is valid state
    REQUIRE(c.UseCount() == 0);
    REQUIRE(a.Expired());
    REQUIRE(b.Expired());
    REQUIRE(c.Expired());
  }

  auto a = WeakPtr<int>();
  {
    auto ptr1 = SharedPtr(new int);
    const auto ptr2 = ptr1;
    auto ptr3 = ptr2;
    a = ptr2;
    const auto b = WeakPtr(ptr3);

    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(ptr1.UseCount() == 3);
    REQUIRE(ptr2.UseCount() == 3);
    REQUIRE(ptr3.UseCount() == 3);
    REQUIRE_FALSE(a.Expired());
    REQUIRE_FALSE(b.Expired());

    ptr1.Reset();
    ptr3.Reset();
    REQUIRE(a.UseCount() == 1);
    REQUIRE(b.UseCount() == 1);
    REQUIRE(ptr1.UseCount() == 0);
    REQUIRE(ptr2.UseCount() == 1);
    REQUIRE(ptr3.UseCount() == 0);
    REQUIRE_FALSE(a.Expired());
    REQUIRE_FALSE(b.Expired());
  }
  REQUIRE(a.Expired());
}

TEST_CASE("Weak Reset", "WeakPtr") {
  auto a = WeakPtr<int>();
  a.Reset();
  REQUIRE(a.UseCount() == 0);
  REQUIRE(a.Expired());

  const auto ptr = SharedPtr(new int);
  auto b = WeakPtr(ptr);
  a = b;
  b.Reset();
  REQUIRE(ptr.UseCount() == 1);
  REQUIRE(a.UseCount() == 1);
  REQUIRE(b.UseCount() == 0);
  REQUIRE(b.Expired());
  REQUIRE_FALSE(a.Expired());

  auto empty_ptr = SharedPtr<int>();
  const auto c = WeakPtr(empty_ptr);
  REQUIRE(c.Expired());

  empty_ptr.Reset(new int);
  REQUIRE(c.Expired());
}

TEST_CASE("Lock", "[WeakPtr]") {
  auto a = WeakPtr<int>();
  a.Reset();
  REQUIRE(a.Lock().Get() == nullptr);

  {
    const auto p = new int;
    const auto ptr = SharedPtr(p);
    auto b = WeakPtr(ptr);
    a = b;
    b.Reset();
    REQUIRE(b.Lock().Get() == nullptr);
    REQUIRE(a.Lock().Get() == p);
  }

  {
    const auto p = new int;
    auto ptr = SharedPtr(p);
    const auto b = WeakPtr(ptr);
    const auto ptr_tmp = b.Lock();

    ptr.Reset();
    REQUIRE(ptr_tmp.Get() == p);
    REQUIRE(ptr_tmp.UseCount() == 1);
  }
}

TEST_CASE("ConstructFromWeak", "[SharedPtr]") {
  {
    const auto wptr = WeakPtr<int>();
    REQUIRE_THROWS_AS(SharedPtr(wptr), BadWeakPtr);  // NOLINT
  }

  {
    const auto ptr = new int;
    const auto init = SharedPtr(ptr);
    const auto weak = WeakPtr(init);
    const auto a = SharedPtr(weak);
    const auto b = SharedPtr(weak);

    REQUIRE_FALSE(weak.Expired());
    REQUIRE(weak.UseCount() == 3);
    REQUIRE(init.UseCount() == 3);
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
  }

  {
    const auto x = new SharedPtr(new int);
    const auto y = WeakPtr(*x);
    delete x;
    REQUIRE(y.Expired());
    REQUIRE_THROWS_AS(SharedPtr(y), BadWeakPtr);  // NOLINT
  }
}

TEST_CASE("Cycle", "[WeakPtr]") {
  struct Node {
    SharedPtr<Node> next;
    WeakPtr<Node> prev;
  };

  auto front = SharedPtr(new Node{});
  auto back = SharedPtr(new Node{});
  front->next = back;
  back->prev = front;
  REQUIRE(front.UseCount() == 1);
  REQUIRE(back.UseCount() == 2);
}
