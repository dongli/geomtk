#ifndef __RLLScalarField_test__
#define __RLLScalarField_test__

#include "RLLScalarField.h"

using namespace geomtk;

class RLLScalarFieldTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    RLLMesh *mesh;
    RLLScalarField *field;

    virtual void SetUp() {
        sphere = new SphereDomain(2);
        mesh = new RLLMesh(*sphere);
        field = new RLLScalarField(*mesh);
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
    }

    virtual void TearDown() {
        delete field;
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLScalarFieldTest, Create) {
    field->create(CENTER, CENTER);
    EXPECT_EQ(10, mesh->getNumGrid(0, CENTER));
    EXPECT_EQ(10, mesh->getNumGrid(1, CENTER));
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            (*field)(0, i, j) = i+j*mesh->getNumGrid(0, CENTER);
        }
    }
    field->applyBndCond(0);
    int n = mesh->getNumGrid(0, CENTER, true);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        EXPECT_EQ(field->data.getLevel(0)(0, j, 0), field->data.getLevel(0)(n-2, j, 0));
        EXPECT_EQ(field->data.getLevel(0)(n-1, j, 0), field->data.getLevel(0)(1, j, 0));
    }
}

#endif
