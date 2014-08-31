#ifndef __RLLMesh_test__
#define __RLLMesh_test__

#include "geomtk.h"

using namespace geomtk;

class RLLMeshTest : public ::testing::Test {
protected:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;

    SphereDomain *domain;
    RLLMesh *mesh;

    virtual void SetUp() {
        domain = new SphereDomain(CLASSIC_PRESSURE_SIGMA);
        mesh = new RLLMesh(*domain);
        mesh->init(10, 10, 5);
    }

    virtual void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLMeshTest, Check1DGridCoords) {
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(mesh->getTotalNumGrid(i, 3), mesh->gridCoords[i].size());
//        for (int j = 0; j < mesh->getTotalNumGrid(i, 3); ++j) {
//            const SphereCoord &x = mesh->getGridCoord(j, i);
//            x.print();
//        }
    }
}

TEST_F(RLLMeshTest, CheckGridCoords) {
    // check dimension sizes
    ASSERT_EQ(10, mesh->getNumGrid(0, GridType::FULL));
    ASSERT_EQ(12, mesh->getNumGrid(0, GridType::FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(0, GridType::HALF));
    ASSERT_EQ(12, mesh->getNumGrid(0, GridType::HALF, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, GridType::FULL));
    ASSERT_EQ(10, mesh->getNumGrid(1, GridType::FULL, true));
    ASSERT_EQ(9, mesh->getNumGrid(1, GridType::HALF));
    ASSERT_EQ(9, mesh->getNumGrid(1, GridType::HALF, true));
    ASSERT_EQ(5, mesh->getNumGrid(2, GridType::FULL));
    ASSERT_EQ(5, mesh->getNumGrid(2, GridType::FULL, true));
    ASSERT_EQ(6, mesh->getNumGrid(2, GridType::HALF));
    ASSERT_EQ(6, mesh->getNumGrid(2, GridType::HALF, true));
    ASSERT_EQ(-M_PI_2, mesh->getGridCoordComp(1, GridType::FULL, 0));
    ASSERT_EQ(M_PI_2, mesh->getGridCoordComp(1, GridType::FULL, mesh->getNumGrid(1, GridType::FULL)-1));
    // check cosine and sine
    vec lonFull = mesh->getGridCoordComps(0, GridType::FULL, true);
    for (int i = -1; i < mesh->getNumGrid(0, GridType::FULL)+1; ++i) {
        ASSERT_EQ(cos(lonFull(i+1)), mesh->getCosLon(GridType::FULL, i));
        ASSERT_EQ(sin(lonFull(i+1)), mesh->getSinLon(GridType::FULL, i));
    }
    vec lonHalf = mesh->getGridCoordComps(0, GridType::HALF, true);
    for (int i = -1; i < mesh->getNumGrid(0, GridType::HALF)+1; ++i) {
        ASSERT_EQ(cos(lonHalf(i+1)), mesh->getCosLon(GridType::HALF, i));
        ASSERT_EQ(sin(lonHalf(i+1)), mesh->getSinLon(GridType::HALF, i));
    }
    vec latFull = mesh->getGridCoordComps(1, GridType::FULL, true);
    for (int j = 0; j < mesh->getNumGrid(1, GridType::FULL, true); ++j) {
        ASSERT_EQ(cos(latFull(j)), mesh->getCosLat(GridType::FULL, j));
        ASSERT_EQ(sin(latFull(j)), mesh->getSinLat(GridType::FULL, j));
        ASSERT_EQ(pow(sin(latFull(j)), 2), mesh->getSinLat2(GridType::FULL, j));
    }
    vec latHalf = mesh->getGridCoordComps(1, GridType::HALF, true);
    for (int j = 0; j < mesh->getNumGrid(1, GridType::HALF, true); ++j) {
        ASSERT_EQ(cos(latHalf(j)), mesh->getCosLat(GridType::HALF, j));
        ASSERT_EQ(sin(latHalf(j)), mesh->getSinLat(GridType::HALF, j));
        ASSERT_EQ(pow(sin(latHalf(j)), 2), mesh->getSinLat2(GridType::HALF, j));
    }
}

TEST_F(RLLMeshTest, Move) {
    SphereCoord x0(3), x1(3);
    double dt = 0.1;
    SphereVelocity v(3);
    RLLMeshIndex idx(3);

    x0.setCoord(0.1, 0.0);
    x0(2) = 0.5;
    idx.locate(*mesh, x0);
    v(0) = 0.1*M_PI;
    v(1) = 0.0;
    v(2) = 0.0;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_EQ(x0(0)+v(0)*dt, x1(0));
    ASSERT_EQ(x0(1), x1(1));

    v(0) = PI2;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x0(0)+v(0)*dt-x1(0)));
    ASSERT_EQ(x0(1), x1(1));

    x0.setCoordComp(1, 89.5*RAD);
    v(0) = 0.0;
    v(1) = 0.0;
    v.transformToPS(x0);
    idx.reset();
    idx.locate(*mesh, x0);
    x0.transformToPS(*domain);
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x0(0)-x1(0)));
    ASSERT_GT(1.0e-15, fabs(x0(1)-x1(1)));

    v(0) = 1.0*RAD;
    v.transformToPS(x0);
    mesh->move(x0, dt, v, idx, x1);
    // TODO: How to test?
}

TEST_F(RLLMeshTest, CheckCellVolume) {
    mesh->setCellVolumes();
    double totalVolume = 0.0;
    for (int i = 0; i < mesh->getTotalNumGrid(Location::CENTER, 2); ++i) {
        totalVolume += mesh->getCellVolume(i);
    }
    ASSERT_GT(1.0e-13, fabs(4*M_PI*domain->getRadius()*domain->getRadius()-totalVolume));
}

#endif
