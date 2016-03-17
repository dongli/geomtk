#ifndef __GEOMTK_RLLField_test__
#define __GEOMTK_RLLField_test__

#include "RLLField.h"

using namespace geomtk;

class RLLFieldTest : public ::testing::Test {
protected:
    typedef RLLField<double, 2> Field;

    const int FULL = RLLStagger::GridType::FULL;
    const int CENTER = RLLStagger::Location::CENTER;

    SphereDomain *sphere;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;
    Field f, g;

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
    ASSERT_EQ(12, f.data->level(timeIdx).n_rows);
    ASSERT_EQ(10, f.data->level(timeIdx).n_cols);
    ASSERT_EQ("f", f.name());
    ASSERT_EQ("1", f.units());
    ASSERT_EQ("f", f.longName());
    ASSERT_EQ(CENTER, f.staggerLocation());
    ASSERT_EQ(mesh, &f.mesh());
}

TEST_F(RLLFieldTest, AccessElements) {
    for (uword i = 0; i < mesh->totalNumGrid(CENTER, f.numDim()); ++i) {
        f(timeIdx, i) = i;
    }
    ASSERT_EQ(0, f.min(timeIdx));
    ASSERT_EQ(mesh->totalNumGrid(CENTER, f.numDim())-1, f.max(timeIdx));
    int l = 0;
    for (auto j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (auto i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            ASSERT_EQ(l++, f(timeIdx)(i, j));
        }
    }
}

TEST_F(RLLFieldTest, AssignmentOperator) {
    for (uword i = 0; i < mesh->totalNumGrid(CENTER, f.numDim()); ++i) {
        f(timeIdx, i) = i;
    }
    ASSERT_EQ(NULL, &g.mesh());
    g = f;
    ASSERT_EQ(mesh, &g.mesh());
    ASSERT_EQ(f.name(), g.name());
    ASSERT_EQ(f.units(), g.units());
    ASSERT_EQ(f.longName(), g.longName());
    ASSERT_EQ(f.staggerLocation(), g.staggerLocation());
    ASSERT_EQ(f.gridType(0), g.gridType(0));
    ASSERT_EQ(f.gridType(1), g.gridType(1));
    for (uword i = 0; i < mesh->totalNumGrid(CENTER, f.numDim()); ++i) {
        ASSERT_EQ(i, g(timeIdx, i));
    }
}

TEST_F(RLLFieldTest, BoundaryCondition) {
    for (auto j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        for (auto i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
            f(timeIdx)(i, j) = i+j*mesh->numGrid(0, FULL);
        }
    }
    f.applyBndCond(timeIdx);
    for (auto j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        ASSERT_EQ(f(timeIdx)(mesh->is(FULL)-1, j), f(timeIdx)(mesh->ie(FULL), j));
        ASSERT_EQ(f(timeIdx)(mesh->ie(FULL)+1, j), f(timeIdx)(mesh->is(FULL), j));
    }
}

#endif // __GEOMTK_RLLField_test__
