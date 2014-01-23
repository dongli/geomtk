#ifndef __StructuredRegrid_test__
#define __StructuredRegrid_test__

#include "StructuredRegrid.h"

using namespace geomtk;

class StructuredRegridTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    StructuredRegrid *regrid;
    StructuredField<double> *f_vector;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        regrid = new StructuredRegrid(*mesh);
        f_vector = new StructuredField<double>(*mesh);

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
        delete regrid;
        delete f_vector;
        delete mesh;
        delete domain;
    }
};

TEST_F(StructuredRegridTest, Run) {
    f_vector->create(VectorField, _2D, C_GRID);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
            (*f_vector)(0, timeIdx, i, j) = 5.0;
        }
    }
    for (int j = 0; j < mesh->getNumGrid(1, EDGE); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            (*f_vector)(1, timeIdx, i, j) = 1.0;
        }
    }
    f_vector->applyBndCond(timeIdx);

    SpaceCoord x(2);
    vec y(2);

    x(0) = 0.1*M_PI;
    x(1) = -0.2*M_PI;
    regrid->run(BILINEAR, timeIdx, *f_vector, x, y);
    ASSERT_EQ(5.0, y(0));
    ASSERT_EQ(1.0, y(1));
}

#endif
