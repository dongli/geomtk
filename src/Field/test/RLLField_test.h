#ifndef __Geomtk_RLLField_test__
#define __Geomtk_RLLField_test__

#include "RLLField.h"

using namespace geomtk;

class RLLFieldTest : public ::testing::Test {
protected:
    typedef RLLField<double, 2> RLLField;

    const int FULL = RLLStagger::GridType::FULL;
    const int CENTER = RLLStagger::Location::CENTER;

    SphereDomain *sphere;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;
    RLLField f, g;

    virtual void SetUp() {
        sphere = new SphereDomain(2);
        mesh = new RLLMesh(*sphere);
        mesh->init(10, 10);
        f.create("f", "1", "f", *mesh, CENTER, 2);
    }

    virtual void TearDown() {
        delete mesh;
        delete sphere;
    }
};

TEST_F(RLLFieldTest, Basics) {
    ASSERT_EQ(12, f.data->getLevel(timeIdx).n_rows);
    ASSERT_EQ(10, f.data->getLevel(timeIdx).n_cols);
    ASSERT_EQ("f", f.getName());
    ASSERT_EQ("1", f.getUnits());
    ASSERT_EQ("f", f.getLongName());
    ASSERT_EQ(CENTER, f.getStaggerLocation());
    ASSERT_EQ(mesh, &f.getMesh());
}

TEST_F(RLLFieldTest, AccessElements) {
    for (int i = 0; i < mesh->getTotalNumGrid(CENTER, f.getNumDim()); ++i) {
        f(timeIdx, i) = i;
    }
    ASSERT_EQ(0, f.min(timeIdx));
    ASSERT_EQ(mesh->getTotalNumGrid(CENTER, f.getNumDim())-1, f.max(timeIdx));
    int l = 0;
    for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            ASSERT_EQ(l++, f(timeIdx, i, j));
        }
    }
}

TEST_F(RLLFieldTest, AssignmentOperator) {
    for (int i = 0; i < mesh->getTotalNumGrid(CENTER, f.getNumDim()); ++i) {
        f(timeIdx, i) = i;
    }
    ASSERT_EQ(NULL, &g.getMesh());
    g = f;
    ASSERT_EQ(mesh, &g.getMesh());
    ASSERT_EQ(f.getName(), g.getName());
    ASSERT_EQ(f.getUnits(), g.getUnits());
    ASSERT_EQ(f.getLongName(), g.getLongName());
    ASSERT_EQ(f.getStaggerLocation(), g.getStaggerLocation());
    ASSERT_EQ(f.getGridType(0), g.getGridType(0));
    ASSERT_EQ(f.getGridType(1), g.getGridType(1));
    for (int i = 0; i < mesh->getTotalNumGrid(CENTER, f.getNumDim()); ++i) {
        ASSERT_EQ(i, g(timeIdx, i));
    }
}

TEST_F(RLLFieldTest, BoundaryCondition) {
    for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            f(timeIdx, i, j) = i+j*mesh->getNumGrid(0, FULL);
        }
    }
    f.applyBndCond(timeIdx);
    for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        ASSERT_EQ(f(timeIdx, mesh->is(FULL)-1, j), f(timeIdx, mesh->ie(FULL), j));
        ASSERT_EQ(f(timeIdx, mesh->ie(FULL)+1, j), f(timeIdx, mesh->is(FULL), j));
    }
}

#endif
