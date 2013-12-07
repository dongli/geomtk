#ifndef __RLLVectorField_test__
#define __RLLVectorField_test__

#include "RLLVectorField.h"

using namespace geomtk;

class RLLVectorFieldTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    RLLVectorField *field;

    virtual void SetUp() {
        domain = new SphereDomain(3);
        mesh = new RLLMesh(*domain);
        field = new RLLVectorField(*mesh);

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
        domain->setAxis(2, 0.0, RIGID, 1.0, RIGID);
        mesh->setGridCoords(2, numLev, fullLev, halfLev);
    }

    virtual void TearDown() {
        delete field;
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLVectorFieldTest, CreateAndSet) {
    field->create(EDGE,   CENTER, CENTER,
                  CENTER, EDGE,   CENTER,
                  CENTER, CENTER, EDGE);
    // check data dimension sizes
    ASSERT_EQ(12, field->data[0].getLevel(0).n_rows);
    ASSERT_EQ(10, field->data[0].getLevel(0).n_cols);
    ASSERT_EQ(5,  field->data[0].getLevel(0).n_slices);
    ASSERT_EQ(12, field->data[1].getLevel(0).n_rows);
    ASSERT_EQ(9,  field->data[1].getLevel(0).n_cols);
    ASSERT_EQ(5,  field->data[1].getLevel(0).n_slices);
    ASSERT_EQ(12, field->data[2].getLevel(0).n_rows);
    ASSERT_EQ(10, field->data[2].getLevel(0).n_cols);
    ASSERT_EQ(6,  field->data[2].getLevel(0).n_slices);
    // set data
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
                (*field)(0, 0, i, j, k) = i+j*mesh->getNumGrid(0, EDGE);
            }
        }
    }
    // apply boundary condition
    field->applyBndCond(0);
    // check virtual boundary grids
    int n = mesh->getNumGrid(0, CENTER, true);
    for (int m = 0; m < domain->getNumDim(); ++m) {
        for (int k = 0; k < mesh->getNumGrid(2, field->getStaggerType(m, 2)); ++k) {
            for (int j = 0; j < mesh->getNumGrid(1, field->getStaggerType(m, 1)); ++j) {
                ASSERT_EQ(field->data[m].getLevel(0)(0,   j, k), field->data[m].getLevel(0)(n-2, j, k));
                ASSERT_EQ(field->data[m].getLevel(0)(n-1, j, k), field->data[m].getLevel(0)(1, j, k));
            }
        }
    }
}

#endif
