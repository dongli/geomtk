#ifndef __RLLRegrid_test__
#define __RLLRegrid_test__

#include "RLLRegrid.h"

using namespace geomtk;

class RLLRegridTest : public ::testing::Test {
protected:
    const int FULL = RLLStagger::GridType::FULL;
    const int HALF = RLLStagger::GridType::HALF;
    const int CENTER = RLLStagger::Location::CENTER;

    SphereDomain *domain;
    RLLMesh *mesh;
    RLLRegrid *regrid;
    RLLVelocityField v;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        regrid = new RLLRegrid(*mesh);

        domain->radius() = 1.0;

        mesh->init(5, 5);
    }

    virtual void TearDown() {
        delete regrid;
        delete mesh;
        delete domain;
    }
};

//     \      -      |      +      |      +      |      +      |      +      |      +      \      -
// -0.4*PI          0.0         0.4*PI        0.8*PI        1.2*PI        1.6*PI        2.0*PI
//        -0.2*PI        0.2*PI        0.6*PI        1.0*PI        1.4*PI        1.8*PI        2.2*PI
//     |      +      |      +      |      +      |      +      |
// -0.5*PI      -0.25*PI          0.0        0.25*PI        0.5*PI
//      -0.375*PI     -0.125*PI      0.125*PI      0.375*PI

TEST_F(RLLRegridTest, Run) {
    v.create(*mesh, true);
    for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (int i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
            v(0)(timeIdx, i, j) = 5.0;
        }
    }
    for (int j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            v(1)(timeIdx, i, j) = 5.0;
        }
    }
    v.applyBndCond(timeIdx);

    SphereCoord x(2);

    x.setCoord(1.9*M_PI, 0.2*M_PI);

    SphereVelocity z(2);
    regrid->run(BILINEAR, timeIdx, v, x, z);
    ASSERT_EQ(5.0, z(0));
    ASSERT_EQ(5.0, z(1));

    // When 'moveOnPole' is true, the interpolated transformed velocity is
    // transformed back.
    z(0) = 0.0;
    z(1) = 0.0;
    x.setCoord(0.1*M_PI, 0.26*M_PI);
    RLLMeshIndex idx(2);
    idx.locate(*mesh, x);
    idx.setMoveOnPole(true);
    regrid->run(BILINEAR, timeIdx, v, x, z, &idx);
    ASSERT_EQ(0.0, z(0));
    ASSERT_EQ(0.0, z(1));

    // When 'moveOnPole' is false, the interpolated transformed velocity is
    // not transformed back.
    z(0) = 0.0;
    z(1) = 0.0;
    x.setCoord(0.1*M_PI, 0.26*M_PI);
    idx.reset();
    idx.locate(*mesh, x);
    regrid->run(BILINEAR, timeIdx, v, x, z, &idx);
    ASSERT_NE(0.0, z(0));
    ASSERT_NE(0.0, z(1));
}

#endif
