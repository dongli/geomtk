#ifndef __Geomtk_RLLField_test__
#define __Geomtk_RLLField_test__

#include "RLLField.h"

using namespace geomtk;

class RLLFieldTest : public ::testing::Test {
protected:
    typedef RLLField<double, 2> RLLField;
    typedef RLLStagger::Location Location;
    typedef RLLStagger::GridType GridType;
    
    SphereDomain *sphere;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;

    virtual void SetUp() {
        sphere = new SphereDomain(2);
        mesh = new RLLMesh(*sphere);
        mesh->init(10, 10);
    }

    virtual void TearDown() {
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLFieldTest, AssignmentOperator) {
    RLLField f, g;
    f.create("f", "1", "f", *mesh, Location::CENTER, 2);
    for (int i = 0; i < mesh->getTotalNumGrid(Location::CENTER, f.getNumDim()); ++i) {
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
    for (int i = 0; i < mesh->getTotalNumGrid(Location::CENTER, f.getNumDim()); ++i) {
        ASSERT_EQ(i, g(timeIdx, i));
    }
}

TEST_F(RLLFieldTest, CheckScalarField) {
    RLLField f;
    f.create("f", "1", "f", *mesh, Location::CENTER, 2);
    // Check data dimensionality.
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
    // Check data 1D indexing.
    ASSERT_EQ(0, f(timeIdx, 0));
    ASSERT_EQ(9, f(timeIdx, 9));
    for (int j = 0; j < mesh->getNumGrid(1, GridType::FULL); ++j) {
        for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
            int cellIdx = i+j*mesh->getNumGrid(0, GridType::FULL);
            ASSERT_EQ(i+j*mesh->getNumGrid(0, GridType::FULL), f(timeIdx, cellIdx));
        }
    }

}

TEST_F(RLLFieldTest, TestVectorFieldHalfLevel) {
    TimeLevelIndex<2> newTimeIdx = timeIdx+1;
    TimeLevelIndex<2> halfTimeIdx = timeIdx+0.5;
    RLLField u, v;

    u.create("u", "m s-1", "u velocity component", *mesh, Location::X_FACE, 2, true);
    v.create("v", "m s-1", "v velocity component", *mesh, Location::Y_FACE, 2, true);

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
