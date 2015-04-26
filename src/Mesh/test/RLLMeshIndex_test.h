#ifndef __GEOMTK_RLLMeshIndex_test__
#define __GEOMTK_RLLMeshIndex_test__

#include "RLLMeshIndex.h"

using namespace geomtk;

class RLLMeshIndexTest : public ::testing::Test {
protected:
    const int FULL = RLLStagger::GridType::FULL;
    const int HALF = RLLStagger::GridType::HALF;
    const int CENTER = RLLStagger::Location::CENTER;
    const int X_FACE = RLLStagger::Location::X_FACE;
    const int Y_FACE = RLLStagger::Location::Y_FACE;
    const int XY_VERTEX = RLLStagger::Location::XY_VERTEX;
    const int VERTEX = RLLStagger::Location::VERTEX;

    SphereDomain *domain;
    RLLMesh *mesh;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        mesh->poleRadius() = 0.1*M_PI;
        mesh->init(5, 5);
    }

    virtual void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLMeshIndexTest, Basic) {
    RLLMeshIndex a(2);

    a.setMoveOnPole(true);
    ASSERT_EQ(true, a.isMoveOnPole());
}

TEST_F(RLLMeshIndexTest, AssignmentOperator) {
    RLLMeshIndex a(3), b(3);

    a.onPole = true;
    a.inPolarCap = true;
    a._pole = NORTH_POLE;
    a.moveOnPole = true;

    b = a;

    ASSERT_EQ(a.onPole, b.onPole);
    ASSERT_EQ(a.inPolarCap, b.inPolarCap);
    ASSERT_EQ(a.pole(), b.pole());
    ASSERT_EQ(a.moveOnPole, b.moveOnPole);
}

//     \      -      |      +      |      +      |      +      |      +      |      +      \      -
// -0.4*PI          0.0         0.4*PI        0.8*PI        1.2*PI        1.6*PI        2.0*PI
//        -0.2*PI        0.2*PI        0.6*PI        1.0*PI        1.4*PI        1.8*PI        2.2*PI
//     |      +      |      +      |      +      |      +      |
// -0.5*PI      -0.25*PI          0.0        0.25*PI        0.5*PI
//      -0.375*PI     -0.125*PI      0.125*PI      0.375*PI

TEST_F(RLLMeshIndexTest, Locate) {
    SphereCoord x(domain->numDim());
    RLLMeshIndex a(domain->numDim());

    x(0) =  0.9*M_PI;
    x(1) = -0.11*M_PI;
    a.locate(*mesh, x);
    ASSERT_EQ(3, a(0, FULL));
    ASSERT_EQ(1, a(1, FULL));
    ASSERT_EQ(2, a(0, HALF));
    ASSERT_EQ(1, a(1, HALF));
    ASSERT_EQ(NOT_POLE, a.pole());
    ASSERT_FALSE(a.isInPolarCap());
    ASSERT_FALSE(a.isOnPole());

    x(0) = 0.14*M_PI;
    a.reset();
    a.locate(*mesh, x);
    ASSERT_EQ(1, a(0, FULL));
    ASSERT_EQ(0, a(0, HALF));

    x(0) = 1.9*M_PI;
    a.reset();
    a.locate(*mesh, x);
    ASSERT_EQ(5, a(0, FULL));
    ASSERT_EQ(5, a(0, HALF));

    x(1) = -0.2*M_PI;
    a.reset();
    a.locate(*mesh, x);
    ASSERT_EQ(1, a(1, FULL));
    ASSERT_EQ(0, a(1, HALF));
    ASSERT_EQ(NOT_POLE, a.pole());
    ASSERT_FALSE(a.isInPolarCap());
    ASSERT_FALSE(a.isOnPole());

    x(1) = -0.39*M_PI;
    a.reset();
    a.locate(*mesh, x);
    ASSERT_EQ(0, a(1, FULL));
    ASSERT_EQ(-1, a(1, HALF));
    ASSERT_EQ(SOUTH_POLE, a.pole());
    ASSERT_TRUE(a.isInPolarCap());
    ASSERT_FALSE(a.isOnPole());

    x(1) = 0.41*M_PI;
    a.reset();
    a.locate(*mesh, x);
    ASSERT_EQ(3, a(1, FULL));
    ASSERT_EQ(3, a(1, HALF));
    ASSERT_EQ(NORTH_POLE, a.pole());
    ASSERT_TRUE(a.isInPolarCap());
    ASSERT_TRUE(a.isOnPole());
}

TEST_F(RLLMeshIndexTest, GetCellIndex) {
    RLLMeshIndex a(domain->numDim());
    int l = 0;
    for (auto j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (auto i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            a.reset();
            a.locate(*mesh, mesh->gridCoord(CENTER, l));
            ASSERT_EQ(l, a.cellIndex(*mesh, CENTER));
            l++;
        }
    }
    l = 0;
    for (auto j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (auto i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
            a.reset();
            a.locate(*mesh, mesh->gridCoord(X_FACE, l));
            ASSERT_EQ(l, a.cellIndex(*mesh, X_FACE));
            l++;
        }
    }
    l = 0;
    for (auto j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        for (auto i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            a.reset();
            a.locate(*mesh, mesh->gridCoord(Y_FACE, l));
            ASSERT_EQ(l, a.cellIndex(*mesh, Y_FACE));
            l++;
        }
    }
    l = 0;
    for (auto j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        for (auto i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
            a.reset();
            a.locate(*mesh, mesh->gridCoord(XY_VERTEX, l));
            ASSERT_EQ(l, a.cellIndex(*mesh, XY_VERTEX));
            l++;
        }
    }
    l = 0;
    for (auto j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        for (auto i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
            a.reset();
            a.locate(*mesh, mesh->gridCoord(VERTEX, l));
            ASSERT_EQ(l, a.cellIndex(*mesh, VERTEX));
            l++;
        }
    }
}

#endif // __GEOMTK_RLLMeshIndex_test__
