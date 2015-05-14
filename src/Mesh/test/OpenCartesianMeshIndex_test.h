#ifndef __GEOMTK_OpenCartesianMeshIndex_test__
#define __GEOMTK_OpenCartesianMeshIndex_test__

#include "geomtk.h"

using namespace geomtk;

class OpenCartesianMeshIndexTest : public ::testing::Test {
protected:
    const int FULL = StructuredStagger::GridType::FULL;
    const int HALF = StructuredStagger::GridType::HALF;
    const int CENTER = StructuredStagger::Location::CENTER;
    const int XY_VERTEX = StructuredStagger::Location::XY_VERTEX;

    CartesianDomain *domain;
    CartesianMesh *mesh;

    virtual void SetUp() {
        domain = new CartesianDomain(3);
        mesh = new CartesianMesh(*domain);

        domain->setAxis(0, "x", "x axis", "m", 0, OPEN, 1, OPEN);
        domain->setAxis(1, "y", "y axis", "m", 0, OPEN, 1, OPEN);
        domain->setAxis(2, "z", "z axis", "m", 0, OPEN, 1, OPEN);

        int nx = 10, ny = 10, nz = 10;
        double dx = 1.0/nx, dy = 1.0/ny, dz = 1.0/nz;
        vec x(nx), y(ny), z(nz);
        for (int i = 0; i < nx; ++i) {
            x[i] = 0.5*dx+i*dx;
        }
        for (int j = 0; j < ny; ++j) {
            y[j] = 0.5*dy+j*dy;
        }
        for (int k = 0; k < nz; ++k) {
            z[k] = 0.5*dz+k*dz;
        }
        mesh->setGridCoordComps(0, nx, x);
        mesh->setGridCoordComps(1, ny, y);
        mesh->setGridCoordComps(2, nz, z);
    }

    virtual void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(OpenCartesianMeshIndexTest, Locate) {
	SpaceCoord x(3);
    CartesianMeshIndex idx(3);

    x.set(0.0, 0.0, 0.0);
    idx.locate(*mesh, x);
    ASSERT_EQ(-1, idx(0, FULL));
    ASSERT_EQ(0, idx(0, HALF));
    ASSERT_EQ(-1, idx(1, FULL));
    ASSERT_EQ(0, idx(1, HALF));
    ASSERT_EQ(-1, idx(2, FULL));
    ASSERT_EQ(0, idx(2, HALF));

    x.set(1.0, 1.0, 1.0);
    idx.locate(*mesh, x);
    ASSERT_EQ(9, idx(0, FULL));
    ASSERT_EQ(9, idx(0, HALF));
    ASSERT_EQ(9, idx(1, FULL));
    ASSERT_EQ(9, idx(1, HALF));
    ASSERT_EQ(9, idx(2, FULL));
    ASSERT_EQ(9, idx(2, HALF));
}

#endif // __GEOMTK_OpenCartesianMeshIndex_test__