#ifndef __Geomtk_RLLField_test__
#define __Geomtk_RLLField_test__

#include "RLLField.h"

using namespace geomtk;

class RLLFieldTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    RLLMesh *mesh;
    RLLField<double> *f_scalar;
    RLLField<double> *f_vector;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        // ---------------------------------------------------------------------
        // create testing variables
        sphere = new SphereDomain(2);
        mesh = new RLLMesh(*sphere);
        f_scalar = new RLLField<double>(*mesh);
        f_vector = new RLLField<double>(*mesh, HAS_HALF_LEVEL);
        // ---------------------------------------------------------------------
        // setup mesh
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
        // ---------------------------------------------------------------------
        // clean testing variables
        delete f_vector;
        delete f_scalar;
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLFieldTest, TestScalarField) {
    f_scalar->create(ScalarField, CENTER, CENTER);
    // -------------------------------------------------------------------------
    // check data dimensionality
    ASSERT_EQ(1, f_scalar->data.size());
    ASSERT_EQ(12, f_scalar->data(0)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f_scalar->data(0)->getLevel(timeIdx).n_cols);
    // -------------------------------------------------------------------------
    // check boundary condition
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            (*f_scalar)(timeIdx, i, j) = i+j*mesh->getNumGrid(0, CENTER);
        }
    }
    f_scalar->applyBndCond(timeIdx);
    int n = mesh->getNumGrid(0, CENTER);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        ASSERT_EQ((*f_scalar)(timeIdx, -1, j), (*f_scalar)(timeIdx, n-1, j));
        ASSERT_EQ((*f_scalar)(timeIdx,  n, j), (*f_scalar)(timeIdx,   0, j));
    }
}

TEST_F(RLLFieldTest, TestVectorFieldHalfLevel) {
    TimeLevelIndex<2> newTimeIdx = timeIdx+1;
    TimeLevelIndex<2> halfTimeIdx = timeIdx+0.5;
    f_vector->create(VectorField, 2, C_GRID);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f_vector->getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f_vector->getStaggerType(m, 0)); ++i) {
                (*f_vector)(m, timeIdx, i, j) = 10;
            }
        }
    }
    f_vector->applyBndCond(timeIdx);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f_vector->getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f_vector->getStaggerType(m, 0)); ++i) {
                (*f_vector)(m, newTimeIdx, i, j) = 20;
            }
        }
    }
    f_vector->applyBndCond(newTimeIdx, UPDATE_HALF_LEVEL);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f_vector->getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f_vector->getStaggerType(m, 0)); ++i) {
                ASSERT_EQ((*f_vector)(m, halfTimeIdx, i, j), 15);
            }
        }
    }
}

TEST_F(RLLFieldTest, TestVectorField) {
    f_vector->create(VectorField, 2, C_GRID);
    // -------------------------------------------------------------------------
    // check data dimensionality
    ASSERT_EQ(2, f_vector->data.size());
    ASSERT_EQ(12, f_vector->data(0)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f_vector->data(0)->getLevel(timeIdx).n_cols);
    ASSERT_EQ(12, f_vector->data(1)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(9, f_vector->data(1)->getLevel(timeIdx).n_cols);
    // -------------------------------------------------------------------------
    // check boundary condition
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
            (*f_vector)(0, timeIdx, i, j) = i+j*mesh->getNumGrid(0, EDGE);
        }
    }
    f_vector->applyBndCond(timeIdx);
    int n = mesh->getNumGrid(0, CENTER);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        ASSERT_EQ((*f_vector)(0, timeIdx, -1, j), (*f_vector)(0, timeIdx, n-1, j));
        ASSERT_EQ((*f_vector)(0, timeIdx,  n, j), (*f_vector)(0, timeIdx,   0, j));
    }
}

#endif
