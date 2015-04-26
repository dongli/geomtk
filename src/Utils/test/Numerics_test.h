#ifndef __GEOMTK_Numerics_test__
#define __GEOMTK_Numerics_test__

#include "geomtk.h"

using namespace geomtk;
using namespace std;

class NumericsTest : public ::testing::Test {
};

TEST_F(NumericsTest, Factors) {
    vector<uword> factors = Numerics::factors(102);
    ASSERT_EQ(factors.size(), 3);
    ASSERT_EQ(factors[0], 2);
    ASSERT_EQ(factors[1], 3);
    ASSERT_EQ(factors[2], 17);
}

#endif // __GEOMTK_Numerics_test__