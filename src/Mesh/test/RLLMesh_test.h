#ifndef __RLLMesh_test__
#define __RLLMesh_test__

#include "RLLMesh.h"

class RLLMeshTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    RLLMesh *mesh;

    virtual void SetUp() {
        sphere = new SphereDomain(3);
        mesh = new RLLMesh(*sphere);
        int numLon = 10;
        double fullLon[numLon], halfLon[numLon];
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        int numLat = 10;
        double fullLat[numLat], halfLat[numLat-1];
        double dlat = M_PI/(numLat-1);
        for (int j = 0; j < numLat; ++j) {
            fullLat[j] = j*dlat-M_PI_2;
        }
        for (int j = 0; j < numLat-1; ++j) {
            halfLat[j] = dlat*0.5+j*dlat-M_PI_2;
        }
        mesh->setGridCoords(1, numLat, fullLat, halfLat);
        int numLev = 5;
        double fullLev[numLev], halfLev[numLev+1];
        double dlev = 1.0/numLev;
        for (int k = 0; k < numLev; ++k) {
            fullLev[k] = dlev*0.5+k*dlev;
        }
        for (int k = 0; k < numLev+1; ++k) {
            halfLev[k] = k*dlev;
        }
        sphere->setAxis(2, 0.0, RIGID, 1.0, RIGID);
        mesh->setGridCoords(2, numLev, fullLev, halfLev);
    }

    virtual void TearDown() {
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLMeshTest, CheckGridCoords) {
    // check dimension sizes
    ASSERT_EQ(10, mesh->getNumGrid(0, CENTER));
    ASSERT_EQ(12, mesh->getNumGrid(0, CENTER, true));
    ASSERT_EQ(10, mesh->getNumGrid(0, EDGE));
    ASSERT_EQ(12, mesh->getNumGrid(0, EDGE, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, CENTER));
    ASSERT_EQ(10, mesh->getNumGrid(1, CENTER, true));
    ASSERT_EQ(9, mesh->getNumGrid(1, EDGE));
    ASSERT_EQ(9, mesh->getNumGrid(1, EDGE, true));
    ASSERT_EQ(5, mesh->getNumGrid(2, CENTER));
    ASSERT_EQ(5, mesh->getNumGrid(2, CENTER, true));
    ASSERT_EQ(6, mesh->getNumGrid(2, EDGE));
    ASSERT_EQ(6, mesh->getNumGrid(2, EDGE, true));
    // check cosine and sine
    vec lonFull = mesh->getGridCoords(0, CENTER, true);
    for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
        ASSERT_EQ(cos(lonFull(i)), mesh->getCosLon(CENTER, i));
        ASSERT_EQ(sin(lonFull(i)), mesh->getSinLon(CENTER, i));
    }
    vec lonHalf = mesh->getGridCoords(0, EDGE, true);
    for (int i = 0; i < mesh->getNumGrid(0, EDGE, true); ++i) {
        ASSERT_EQ(cos(lonHalf(i)), mesh->getCosLon(EDGE, i));
        ASSERT_EQ(sin(lonHalf(i)), mesh->getSinLon(EDGE, i));
    }
    vec latFull = mesh->getGridCoords(1, CENTER, true);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER, true); ++j) {
        ASSERT_EQ(cos(latFull(j)), mesh->getCosLat(CENTER, j));
        ASSERT_EQ(sin(latFull(j)), mesh->getSinLat(CENTER, j));
        ASSERT_EQ(pow(sin(latFull(j)), 2), mesh->getSinLat2(CENTER, j));
    }
    vec latHalf = mesh->getGridCoords(1, EDGE, true);
    for (int j = 0; j < mesh->getNumGrid(1, EDGE, true); ++j) {
        ASSERT_EQ(cos(latHalf(j)), mesh->getCosLat(EDGE, j));
        ASSERT_EQ(sin(latHalf(j)), mesh->getSinLat(EDGE, j));
        ASSERT_EQ(pow(sin(latHalf(j)), 2), mesh->getSinLat2(EDGE, j));
    }
}

#endif
