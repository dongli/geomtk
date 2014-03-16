#ifndef __StructuredRegrid_test__
#define __StructuredRegrid_test__

#include "StructuredRegrid.h"

using namespace geomtk;

class StructuredRegridTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    StructuredRegrid *regrid;
    StructuredField<double> f;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        regrid = new StructuredRegrid(*mesh);

        int numLon = 5;
        vec fullLon(numLon), halfLon(numLon);
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        int numLat = 5;
        vec fullLat(numLat), halfLat(numLat-1);
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
        delete mesh;
        delete domain;
    }
};

TEST_F(StructuredRegridTest, Run) {
    f.create("", "", "", *mesh, StructuredStagger::Location::X_FACE);
    for (int j = 0; j < mesh->getNumGrid(1, StructuredStagger::GridType::FULL); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, StructuredStagger::GridType::HALF); ++i) {
            f(timeIdx, i, j) = 5.0;
        }
    }
    f.applyBndCond(timeIdx);

    SpaceCoord x(2);
    double y;

    x(0) = 0.1*M_PI;
    x(1) = -0.2*M_PI;
    regrid->run(BILINEAR, timeIdx, f, x, y);
    ASSERT_EQ(5.0, y);
}

#endif
