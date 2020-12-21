#include "emlabcpp/algorithm.h"
#include "emlabcpp/iterator.h"
#include "emlabcpp/iterators/numeric.h"

#include <gtest/gtest.h>

using namespace emlabcpp;

TEST(numeric_iterator, basic) {
        auto r = range(-5.f, 6.f);
        EXPECT_EQ(sum(r), 0.f);

        auto r2 = range(10);
        EXPECT_EQ(max_elem(r2), 9);
        EXPECT_EQ(min_elem(r2), 0);
}

TEST(numeric_iterator, operators) {
        auto r = range(10);

        EXPECT_EQ(r.begin() + 10, r.end());
}

TEST(numeric_itertor, cmp) {
        numeric_iterator a{2};
        numeric_iterator b{3};
        numeric_iterator c{3};

        EXPECT_EQ(b, c);
        EXPECT_NE(a, b);
        EXPECT_LT(a, b);
        EXPECT_GT(c, a);
}
