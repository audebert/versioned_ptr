#include "versioned_ptr.h"

#include "gtest/gtest.h"

class State {
public:
  State(int a) : a(a) {}

  int a;
};

TEST(VersionedPtr, make_versioned) {
  auto v = make_versioned<State>(42);
  EXPECT_EQ(v->a, 42);
}

TEST(VersionedPtr, save_restore) {
  auto v = VersionedPtr<State>(std::make_unique<State>(42));

  EXPECT_EQ(v->a, 42);

  EXPECT_FALSE(v.can_restore());
  v.save();
  EXPECT_TRUE(v.can_restore());

  v->a = 1337;
  EXPECT_EQ(v->a, 1337);

  v.restore();
  EXPECT_FALSE(v.can_restore());

  EXPECT_EQ(v->a, 42);
}

TEST(VersionedPtr, clear) {
  auto v = VersionedPtr<State>(std::make_unique<State>(42));
  v.save();
  v.save();
  v.save();
  v.clear();
  EXPECT_EQ(v->a, 42);
  EXPECT_FALSE(v.can_restore());
}

TEST(VersionedPtr, multiple_restore) {
  auto v = VersionedPtr<State>(std::make_unique<State>(42));
  v.save();    // a = 42, 1 save
  v->a = 1337; // mutate current
  v.save();    // a = 1337, 2 saves
  v->a = 1234; // mutate current
  v.save();    // a = 1234, 3 saves
  v.restore(); // a = 1234, 2 save
  EXPECT_EQ(v->a, 1234);
  v.restore(); // a = 1337, 1 save
  EXPECT_EQ(v->a, 1337);

  // ASAN will verify that no memory is leaked
}
