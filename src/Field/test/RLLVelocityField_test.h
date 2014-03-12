#ifndef __RLLVelocityField_test__
#define __RLLVelocityField_test__

#include "RLLVelocityField.h"

using namespace geomtk;

class RLLVelocityFieldTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    RLLVelocityField *v;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        v = new RLLVelocityField(*mesh);

        int numLon = 5;
        double fullLon[numLon], halfLon[numLon];
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        int numLat = 5;
        double fullLat[numLat], halfLat[numLat-1];
        double dlat = M_PI/(numLat-1);
        for (int j = 0; j < numLat; ++j) {
            fullLat[j] = j*dlat-M_PI_2;
        }
        for (int j = 0; j < numLat-1; ++j) {
            halfLat[j] = dlat*0.5+j*dlat-M_PI_2;
        }
        mesh->setGridCoords(1, numLat, fullLat, halfLat);
    }

    virtual void TearDown() {
        delete v;
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLVelocityFieldTest, CreateAndSet) {
    v->create(_2D, C_GRID);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
            (*v)(0, timeIdx, i, j) = 5.0;
        }
    }
    for (int j = 0; j < mesh->getNumGrid(1, EDGE); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            (*v)(1, timeIdx, i, j) = 5.0;
        }
    }
    v->applyBndCond(timeIdx);
    // -------------------------------------------------------------------------
    // check boundary condition
    int n = mesh->getNumGrid(0, CENTER);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        ASSERT_EQ(5.0, (*v)(0, timeIdx, -1, j));
        ASSERT_EQ(5.0, (*v)(0, timeIdx,  n, j));
    }
    for (int j = 0; j < mesh->getNumGrid(1, EDGE); ++j) {
        ASSERT_EQ(5.0, (*v)(1, timeIdx, -1, j));
        ASSERT_EQ(5.0, (*v)(1, timeIdx,  n, j));
    }
    for (int m = 0; m < domain->getNumDim(); ++m) {
        for (int l = 0; l < 2; ++l) {
            ASSERT_EQ(v->rings[l].getOriginalData(m, timeIdx, -1),
                      v->rings[l].getOriginalData(m, timeIdx, n-1));
            ASSERT_EQ(v->rings[l].getOriginalData(m, timeIdx, n),
                      v->rings[l].getOriginalData(m, timeIdx, 0));
            ASSERT_EQ(v->rings[l].getTransformedData(m, timeIdx, -1),
                      v->rings[l].getTransformedData(m, timeIdx, n-1));
            ASSERT_EQ(v->rings[l].getTransformedData(m, timeIdx, n),
                      v->rings[l].getTransformedData(m, timeIdx, 0));
        }
    }
    // -------------------------------------------------------------------------
    // check original velocity on the rings
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            ASSERT_EQ(v->rings[l].getOriginalData(0, timeIdx, i),
                      ((*v)(0, timeIdx, i, j)+(*v)(0, timeIdx, i-1, j))*0.5);
            ASSERT_EQ(v->rings[l].getOriginalData(1, timeIdx, i),
                      ((*v)(1, timeIdx, i, j)+(*v)(1, timeIdx, j+1, j))*0.5);
        }
    }
    // -------------------------------------------------------------------------
    // check transformed velocity on the rings
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            SphereCoord x(2);
            x.setCoord(mesh->getGridCoordComp(0, CENTER, i),
                       mesh->getGridCoordComp(1, CENTER, j));
            SphereVelocity u(2);
            u = v->rings[l].vr(i+1, 0)->getLevel(timeIdx);
            u.transformFromPS(x);
            ASSERT_GT(1.0e-15, fabs(v->rings[l].getOriginalData(0, timeIdx, i)-u(0)));
            ASSERT_GT(1.0e-15, fabs(v->rings[l].getOriginalData(1, timeIdx, i)-u(1)));
        }
    }
}

#endif
