#ifndef __Geomtk_RLLField_test__
#define __Geomtk_RLLField_test__

#include "RLLField.h"

using namespace geomtk;

class RLLFieldTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        // ---------------------------------------------------------------------
        // create testing variables
        sphere = new SphereDomain(2);
        mesh = new RLLMesh(*sphere);
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
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLFieldTest, CheckGridType) {
    RLLField<double> f1, f2, f3, f4, f5;
    f1.create("", "", "", *mesh, ScalarField, CENTER, CENTER);
    ASSERT_EQ(A_GRID, f1.gridType);
    f2.create("", "", "", *mesh, VectorField, CENTER, CENTER, CENTER, CENTER);
    ASSERT_EQ(A_GRID, f2.gridType);
    f3.create("", "", "", *mesh, VectorField, EDGE, CENTER, CENTER, EDGE);
    ASSERT_EQ(C_GRID, f3.gridType);
    f4.create("", "", "", *mesh, VectorField, CENTER, CENTER, CENTER,
              CENTER, CENTER, CENTER, CENTER, CENTER, CENTER);
    ASSERT_EQ(A_GRID, f4.gridType);
    f5.create("", "", "", *mesh, VectorField, EDGE, CENTER, CENTER,
              CENTER, EDGE, CENTER, CENTER, CENTER, EDGE);
    ASSERT_EQ(C_GRID, f5.gridType);

}

TEST_F(RLLFieldTest, CheckScalarField) {
    RLLField<double> f;
    f.create("", "", "", *mesh, ScalarField, CENTER, CENTER);
    // -------------------------------------------------------------------------
    // check data dimensionality
    ASSERT_EQ(1, f.data.size());
    ASSERT_EQ(12, f.data(0)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f.data(0)->getLevel(timeIdx).n_cols);
    // -------------------------------------------------------------------------
    // check boundary condition
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            f(timeIdx, i, j) = i+j*mesh->getNumGrid(0, CENTER);
        }
    }
    f.applyBndCond(timeIdx);
    int n = mesh->getNumGrid(0, CENTER);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        ASSERT_EQ(f(timeIdx, -1, j), f(timeIdx, n-1, j));
        ASSERT_EQ(f(timeIdx,  n, j), f(timeIdx,   0, j));
    }
    // -------------------------------------------------------------------------
    // check data indexing
    ASSERT_EQ(0, f(timeIdx, 0));
    ASSERT_EQ(9, f(timeIdx, 9));
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            int cellIdx = i+j*mesh->getNumGrid(0, CENTER);
            ASSERT_EQ(i+j*mesh->getNumGrid(0, CENTER), f(timeIdx, cellIdx));
        }
    }

}

TEST_F(RLLFieldTest, TestVectorFieldHalfLevel) {
    TimeLevelIndex<2> newTimeIdx = timeIdx+1;
    TimeLevelIndex<2> halfTimeIdx = timeIdx+0.5;
    RLLField<double> f;
    f.create("", "", "", *mesh, VectorField, _2D, C_GRID, true);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f.getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f.getStaggerType(m, 0)); ++i) {
                f(m, timeIdx, i, j) = 10;
            }
        }
    }
    f.applyBndCond(timeIdx);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f.getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f.getStaggerType(m, 0)); ++i) {
                f(m, newTimeIdx, i, j) = 20;
            }
        }
    }
    f.applyBndCond(newTimeIdx, UPDATE_HALF_LEVEL);
    for (int m = 0; m < 2; ++m) {
        for (int j = 0; j < mesh->getNumGrid(1, f.getStaggerType(m, 1)); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, f.getStaggerType(m, 0)); ++i) {
                ASSERT_EQ(f(m, halfTimeIdx, i, j), 15);
            }
        }
    }
}

TEST_F(RLLFieldTest, TestVectorField) {
    RLLField<double> f;
    f.create("", "", "", *mesh, VectorField, _2D, C_GRID);
    // -------------------------------------------------------------------------
    // check data dimensionality
    ASSERT_EQ(2, f.data.size());
    ASSERT_EQ(12, f.data(0)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f.data(0)->getLevel(timeIdx).n_cols);
    ASSERT_EQ(12, f.data(1)->getLevel(timeIdx).n_rows);
    ASSERT_EQ(9, f.data(1)->getLevel(timeIdx).n_cols);
    // -------------------------------------------------------------------------
    // check boundary condition
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
            f(0, timeIdx, i, j) = i+j*mesh->getNumGrid(0, EDGE);
        }
    }
    f.applyBndCond(timeIdx);
    int n = mesh->getNumGrid(0, CENTER);
    for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
        ASSERT_EQ(f(0, timeIdx, -1, j), f(0, timeIdx, n-1, j));
        ASSERT_EQ(f(0, timeIdx,  n, j), f(0, timeIdx,   0, j));
    }
}

#endif
