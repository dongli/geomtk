#ifndef __RLLMesh_test__
#define __RLLMesh_test__

#include "geomtk.h"

using namespace geomtk;

class RLLMeshTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;

    virtual void SetUp() {
        domain = new SphereDomain(CLASSIC_PRESSURE_SIGMA);
        mesh = new RLLMesh(*domain);
        int numLon = 10;
        vec fullLon(numLon), halfLon(numLon);
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        int numLat = 10;
        vec fullLat(numLat), halfLat(numLat-1);
        double dlat = M_PI/(numLat-1);
        for (int j = 0; j < numLat; ++j) {
            fullLat[j] = j*dlat-M_PI_2;
        }
        for (int j = 0; j < numLat-1; ++j) {
            halfLat[j] = dlat*0.5+j*dlat-M_PI_2;
        }
        mesh->setGridCoords(1, numLat, fullLat, halfLat);
        int numLev = 5;
        vec fullLev(numLev), halfLev(numLev+1);
        double dlev = 1.0/numLev;
        for (int k = 0; k < numLev; ++k) {
            fullLev[k] = dlev*0.5+k*dlev;
        }
        for (int k = 0; k < numLev+1; ++k) {
            halfLev[k] = k*dlev;
        }
        mesh->setGridCoords(2, numLev, fullLev, halfLev);
    }

    virtual void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLMeshTest, CheckGridCoords) {
    // check dimension sizes
    ASSERT_EQ(10, mesh->getNumGrid(0, StructuredStagger::GridType::FULL));
    ASSERT_EQ(12, mesh->getNumGrid(0, StructuredStagger::GridType::FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(0, StructuredStagger::GridType::HALF));
    ASSERT_EQ(12, mesh->getNumGrid(0, StructuredStagger::GridType::HALF, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, StructuredStagger::GridType::FULL));
    ASSERT_EQ(10, mesh->getNumGrid(1, StructuredStagger::GridType::FULL, true));
    ASSERT_EQ(9, mesh->getNumGrid(1, StructuredStagger::GridType::HALF));
    ASSERT_EQ(9, mesh->getNumGrid(1, StructuredStagger::GridType::HALF, true));
    ASSERT_EQ(5, mesh->getNumGrid(2, StructuredStagger::GridType::FULL));
    ASSERT_EQ(5, mesh->getNumGrid(2, StructuredStagger::GridType::FULL, true));
    ASSERT_EQ(6, mesh->getNumGrid(2, StructuredStagger::GridType::HALF));
    ASSERT_EQ(6, mesh->getNumGrid(2, StructuredStagger::GridType::HALF, true));
    ASSERT_EQ(-M_PI_2, mesh->getGridCoordComp(1, StructuredStagger::GridType::FULL, 0));
    ASSERT_EQ(M_PI_2, mesh->getGridCoordComp(1, StructuredStagger::GridType::FULL, mesh->getNumGrid(1, StructuredStagger::GridType::FULL)-1));
    // check cosine and sine
    vec lonFull = mesh->getGridCoords(0, StructuredStagger::GridType::FULL, true);
    for (int i = -1; i < mesh->getNumGrid(0, StructuredStagger::GridType::FULL)+1; ++i) {
        ASSERT_EQ(cos(lonFull(i+1)), mesh->getCosLon(StructuredStagger::GridType::FULL, i));
        ASSERT_EQ(sin(lonFull(i+1)), mesh->getSinLon(StructuredStagger::GridType::FULL, i));
    }
    vec lonHalf = mesh->getGridCoords(0, StructuredStagger::GridType::HALF, true);
    for (int i = -1; i < mesh->getNumGrid(0, StructuredStagger::GridType::HALF)+1; ++i) {
        ASSERT_EQ(cos(lonHalf(i+1)), mesh->getCosLon(StructuredStagger::GridType::HALF, i));
        ASSERT_EQ(sin(lonHalf(i+1)), mesh->getSinLon(StructuredStagger::GridType::HALF, i));
    }
    vec latFull = mesh->getGridCoords(1, StructuredStagger::GridType::FULL, true);
    for (int j = 0; j < mesh->getNumGrid(1, StructuredStagger::GridType::FULL, true); ++j) {
        ASSERT_EQ(cos(latFull(j)), mesh->getCosLat(StructuredStagger::GridType::FULL, j));
        ASSERT_EQ(sin(latFull(j)), mesh->getSinLat(StructuredStagger::GridType::FULL, j));
        ASSERT_EQ(pow(sin(latFull(j)), 2), mesh->getSinLat2(StructuredStagger::GridType::FULL, j));
    }
    vec latHalf = mesh->getGridCoords(1, StructuredStagger::GridType::HALF, true);
    for (int j = 0; j < mesh->getNumGrid(1, StructuredStagger::GridType::HALF, true); ++j) {
        ASSERT_EQ(cos(latHalf(j)), mesh->getCosLat(StructuredStagger::GridType::HALF, j));
        ASSERT_EQ(sin(latHalf(j)), mesh->getSinLat(StructuredStagger::GridType::HALF, j));
        ASSERT_EQ(pow(sin(latHalf(j)), 2), mesh->getSinLat2(StructuredStagger::GridType::HALF, j));
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
    double volume;
    mesh->setCellVolumes();
    volume = mesh->getCellVolume(0);
}

#endif
