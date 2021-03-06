#include "emlabcpp/physical_quantity.h"

#include <gtest/gtest.h>

using namespace emlabcpp;

// NOLINTNEXTLINE
TEST( physical_quantity_test, basic )
{
        unitless u{ 0.5f };

        EXPECT_EQ( *u, 0.5f );
        EXPECT_EQ( u += unitless{ 1.f }, unitless{ 1.5f } );
        EXPECT_EQ( u -= unitless{ 1.f }, unitless{ 0.5f } );
        EXPECT_EQ( u /= 2.f, unitless{ 0.25f } );
        EXPECT_EQ( u *= 2.f, unitless{ 0.5f } );
        EXPECT_EQ( float{ u }, 0.5f );
        EXPECT_EQ( u + unitless{ 1.f }, unitless{ 1.5f } );
        EXPECT_EQ( u - unitless{ 1.f }, unitless{ -0.5f } );
        EXPECT_EQ( -u, unitless{ -0.5f } );
        EXPECT_TRUE( u == unitless{ 0.5f } );
        EXPECT_FALSE( u == unitless{ 0.25f } );
        EXPECT_TRUE( u < unitless{ 0.75f } );
        EXPECT_FALSE( u < unitless{ 0.5f } );
        EXPECT_EQ( u * 0.5f, unitless{ 0.25f } );
        EXPECT_EQ( u / 0.5f, unitless{ 1.f } );
        EXPECT_EQ( abs( unitless{ -0.5f } ), unitless{ 0.5f } );
        EXPECT_EQ( cos( unitless{ 0.f } ), 1. );
        EXPECT_EQ( sin( unitless{ 0.f } ), 0. );
        EXPECT_EQ( max( unitless{ 0.5f }, unitless{ 1.f } ), unitless{ 1.f } );
        EXPECT_EQ( min( unitless{ 0.5f }, unitless{ 1.f } ), unitless{ 0.5f } );
        EXPECT_EQ( pow< 2 >( unitless{ 2.f } ), unitless{ 4.f } );
}

// NOLINTNEXTLINE
TEST( physical_quantity_test, specializations )
{
        EXPECT_EQ(
            *std::numeric_limits< unitless >::lowest(), std::numeric_limits< float >::lowest() );
        EXPECT_EQ( *std::numeric_limits< unitless >::min(), std::numeric_limits< float >::min() );
        EXPECT_EQ( *std::numeric_limits< unitless >::max(), std::numeric_limits< float >::max() );
}
