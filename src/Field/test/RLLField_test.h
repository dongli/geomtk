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
    }

    virtual void TearDown() {
        // ---------------------------------------------------------------------
        // clean testing variables
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLFieldTest, AssignmentOperator) {
    RLLField<double, 2> f, g;
    f.create("1", "2", "3", *mesh, RLLStagger::Location::CENTER, 2, false);
    for (int i = 0; i < mesh->getTotalNumGrid(RLLStagger::Location::CENTER, f.getNumDim()); ++i) {
        f(timeIdx, i) = i;
    }
    ASSERT_EQ(NULL, &g.getMesh());
    g = f;
    ASSERT_EQ(mesh, &g.getMesh());
    ASSERT_EQ(f.getName(), g.getName());
    ASSERT_EQ(f.getLongName(), g.getLongName());
    ASSERT_EQ(f.getUnits(), g.getUnits());
    ASSERT_EQ(f.getStaggerLocation(), g.getStaggerLocation());
    ASSERT_EQ(f.getGridType(0), g.getGridType(0));
    ASSERT_EQ(f.getGridType(1), g.getGridType(1));
    for (int i = 0; i < mesh->getTotalNumGrid(RLLStagger::Location::CENTER, f.getNumDim()); ++i) {
        ASSERT_EQ(i, g(timeIdx, i));
    }
}

TEST_F(RLLFieldTest, CheckScalarField) {
    RLLField<double, 2> f;
    f.create("", "", "", *mesh, RLLStagger::Location::CENTER, 2, false);
    // -------------------------------------------------------------------------
    // check data dimensionality
    ASSERT_EQ(12, f.data->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f.data->getLevel(timeIdx).n_cols);
    // -------------------------------------------------------------------------
    // check boundary condition
    for (int j = 0; j < mesh->getNumGrid(1, RLLStagger::GridType::FULL); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, RLLStagger::GridType::FULL); ++i) {
            f(timeIdx, i, j) = i+j*mesh->getNumGrid(0, RLLStagger::GridType::FULL);
        }
    }
    f.applyBndCond(timeIdx);
    int n = mesh->getNumGrid(0, RLLStagger::GridType::FULL);
    for (int j = 0; j < mesh->getNumGrid(1, RLLStagger::GridType::FULL); ++j) {
        ASSERT_EQ(f(timeIdx, -1, j), f(timeIdx, n-1, j));
        ASSERT_EQ(f(timeIdx,  n, j), f(timeIdx,   0, j));
    }
    // -------------------------------------------------------------------------
    // check data indexing
    ASSERT_EQ(0, f(timeIdx, 0));
    ASSERT_EQ(9, f(timeIdx, 9));
    for (int j = 0; j < mesh->getNumGrid(1, RLLStagger::GridType::FULL); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, RLLStagger::GridType::FULL); ++i) {
            int cellIdx = i+j*mesh->getNumGrid(0, RLLStagger::GridType::FULL);
            ASSERT_EQ(i+j*mesh->getNumGrid(0, RLLStagger::GridType::FULL), f(timeIdx, cellIdx));
        }
    }

}

TEST_F(RLLFieldTest, TestVectorFieldHalfLevel) {
    TimeLevelIndex<2> newTimeIdx = timeIdx+1;
    TimeLevelIndex<2> halfTimeIdx = timeIdx+0.5;
    RLLField<double, 2> u, v;

    u.create("u", "", "", *mesh, RLLStagger::Location::X_FACE, 2, true);
    v.create("v", "", "", *mesh, RLLStagger::Location::Y_FACE, 2, true);

    ASSERT_EQ(12, u.data->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, u.data->getLevel(timeIdx).n_cols);
    ASSERT_EQ(12, v.data->getLevel(timeIdx).n_rows);
    ASSERT_EQ(9, v.data->getLevel(timeIdx).n_cols);

    for (int j = 0; j < mesh->getNumGrid(1, u.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, u.getGridType(0)); ++i) {
            u(timeIdx, i, j) = 10;
        }
    }
    u.applyBndCond(timeIdx);
    
    for (int j = 0; j < mesh->getNumGrid(1, v.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, v.getGridType(0)); ++i) {
            v(timeIdx, i, j) = 10;
        }
    }
    v.applyBndCond(timeIdx);

    int n = mesh->getNumGrid(0, u.getGridType(0));
    for (int j = 0; j < mesh->getNumGrid(1, u.getGridType(1)); ++j) {
        ASSERT_EQ(u(timeIdx, -1, j), u(timeIdx, n-1, j));
        ASSERT_EQ(u(timeIdx,  n, j), u(timeIdx,   0, j));
    }

    for (int j = 0; j < mesh->getNumGrid(1, u.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, u.getGridType(0)); ++i) {
            u(newTimeIdx, i, j) = 20;
        }
    }
    u.applyBndCond(newTimeIdx, UPDATE_HALF_LEVEL);
    
    for (int j = 0; j < mesh->getNumGrid(1, v.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, v.getGridType(0)); ++i) {
            v(newTimeIdx, i, j) = 20;
        }
    }
    v.applyBndCond(newTimeIdx, UPDATE_HALF_LEVEL);

    for (int j = 0; j < mesh->getNumGrid(1, u.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, u.getGridType(0)); ++i) {
            ASSERT_EQ(u(halfTimeIdx, i, j), 15);
        }
    }
    for (int j = 0; j < mesh->getNumGrid(1, v.getGridType(1)); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, v.getGridType(0)); ++i) {
            ASSERT_EQ(v(halfTimeIdx, i, j), 15);
        }
    }
}

#endif
