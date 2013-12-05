#ifndef __RLLVelocityField_test__
#define __RLLVelocityField_test__

#include "RLLVelocityField.h"

class RLLVelocityFieldTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    RLLVelocityField *v;

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
    v->create(EDGE,   CENTER, CENTER, EDGE);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
            (*v)(0, 0, i, j) = 5.0;
        }
    }
    for (int j = 0; j < mesh->getNumGrid(1, EDGE); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            (*v)(0, 1, i, j) = 5.0;
        }
    }
    v->applyBndCond(0);
    // check boundary grids
    int n = mesh->getNumGrid(0, CENTER);
    for (int m = 0; m < domain->getNumDim(); ++m) {
        for (int l = 0; l < 2; ++l) {
            ASSERT_EQ(v->rings[l].getOriginalData(0, m, -1), v->rings[l].getOriginalData(0, m, n-1));
            ASSERT_EQ(v->rings[l].getOriginalData(0, m,  n), v->rings[l].getOriginalData(0, m, 0));
            ASSERT_EQ(v->rings[l].getTransformedData(0, m, -1), v->rings[l].getTransformedData(0, m, n-1));
            ASSERT_EQ(v->rings[l].getTransformedData(0, m,  n), v->rings[l].getTransformedData(0, m, 0));
        }
    }
    // check rings
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            ASSERT_EQ(v->rings[l].getOriginalData(0, 0, i), ((*v)(0, 0, i, j)+(*v)(0, 0, i-1, j))*0.5);
            ASSERT_EQ(v->rings[l].getOriginalData(0, 1, i), ((*v)(0, 1, i, j)+(*v)(0, 1, j+1, j))*0.5);
        }
    }
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            SphereCoord x(2);
            x(0) = mesh->getGridCoord(0, CENTER, i);
            x(1) = mesh->getGridCoord(1, CENTER, j);
            SphereVelocity u(2);
            u = v->rings[l].vr[i+1][0].get(0);
            u.transformFromPS(x);
            ASSERT_GT(1.0e-15, abs(v->rings[l].vr[i+1][0].get(0)(0)-u(0)));
            ASSERT_GT(1.0e-15, abs(v->rings[l].vr[i+1][0].get(0)(1)-u(1)));
        }
    }}

#endif
