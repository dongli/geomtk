#ifndef __RLLMeshIndex_test__
#define __RLLMeshIndex_test__

#include "RLLMeshIndex.h"

using namespace geomtk;

class RLLMeshIndexTest : public ::testing::Test {
protected:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;

    SphereDomain *domain;
    RLLMesh *mesh;
    RLLMeshIndex *index;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        index = new RLLMeshIndex(domain->getNumDim());

        mesh->setPoleRadius(0.1*M_PI);
        mesh->init(5, 5);
    }

    virtual void TearDown() {
        delete index;
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLMeshIndexTest, AssignmentOperator) {
    RLLMeshIndex a(3), b(3);

    a.onPole = true;
    a.inPolarCap = true;
    a.pole = NORTH_POLE;
    a.moveOnPole = true;

    b = a;

    ASSERT_EQ(a.onPole, b.onPole);
    ASSERT_EQ(a.inPolarCap, b.inPolarCap);
    ASSERT_EQ(a.pole, b.pole);
    ASSERT_EQ(a.moveOnPole, b.moveOnPole);
}

TEST_F(RLLMeshIndexTest, Basic) {
    RLLMeshIndex a(2);

    a.setMoveOnPole(true);
    ASSERT_EQ(true, a.isMoveOnPole());
}

//     \      -      |      +      |      +      |      +      |      +      |      +      \      -
// -0.4*PI          0.0         0.4*PI        0.8*PI        1.2*PI        1.6*PI        2.0*PI
//        -0.2*PI        0.2*PI        0.6*PI        1.0*PI        1.4*PI        1.8*PI        2.2*PI
//     |      +      |      +      |      +      |      +      |
// -0.5*PI      -0.25*PI          0.0        0.25*PI        0.5*PI
//      -0.375*PI     -0.125*PI      0.125*PI      0.375*PI

TEST_F(RLLMeshIndexTest, Locate) {
    SphereCoord x(domain->getNumDim());

    x(0) =  0.9*M_PI;
    x(1) = -0.11*M_PI;
    index->locate(*mesh, x);
    ASSERT_EQ(2, (*index)(0, GridType::FULL));
    ASSERT_EQ(1, (*index)(1, GridType::FULL));
    ASSERT_EQ(1, (*index)(0, GridType::HALF));
    ASSERT_EQ(1, (*index)(1, GridType::HALF));
    ASSERT_EQ(NOT_POLE, index->getPole());
    ASSERT_FALSE(index->isInPolarCap());
    ASSERT_FALSE(index->isOnPole());

    x(0) = 0.14*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(0, (*index)(0, GridType::FULL));
    ASSERT_EQ(-1, (*index)(0, GridType::HALF));

    x(0) = 1.9*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(4, (*index)(0, GridType::FULL));
    ASSERT_EQ(4, (*index)(0, GridType::HALF));

    x(1) = -0.2*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(1, (*index)(1, GridType::FULL));
    ASSERT_EQ(0, (*index)(1, GridType::HALF));
    ASSERT_EQ(NOT_POLE, index->getPole());
    ASSERT_FALSE(index->isInPolarCap());
    ASSERT_FALSE(index->isOnPole());

    x(1) = -0.39*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(0, (*index)(1, GridType::FULL));
    ASSERT_EQ(-1, (*index)(1, GridType::HALF));
    ASSERT_EQ(SOUTH_POLE, index->getPole());
    ASSERT_TRUE(index->isInPolarCap());
    ASSERT_FALSE(index->isOnPole());

    x(1) = 0.41*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(3, (*index)(1, GridType::FULL));
    ASSERT_EQ(3, (*index)(1, GridType::HALF));
    ASSERT_EQ(NORTH_POLE, index->getPole());
    ASSERT_TRUE(index->isInPolarCap());
    ASSERT_TRUE(index->isOnPole());
}

TEST_F(RLLMeshIndexTest, GetIndex) {
    (*index)(0, GridType::FULL) = 0;
    (*index)(1, GridType::FULL) = 0;
    ASSERT_EQ(0, index->getIndex(*mesh, Location::CENTER));
    (*index)(0, GridType::FULL) = 3;
    (*index)(1, GridType::FULL) = 2;
    ASSERT_EQ(13, index->getIndex(*mesh, Location::CENTER));
    (*index)(0, GridType::HALF) = 2;
    (*index)(1, GridType::HALF) = 3;
    ASSERT_EQ(17, index->getIndex(*mesh, Location::XY_VERTEX));
}

#endif
