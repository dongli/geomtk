#ifndef __Geomtk_OpenCartesainMesh_test__
#define __Geomtk_OpenCartesainMesh_test__

#include "geomtk.h"

using namespace geomtk;

class OpenCartesianMeshTest : public ::testing::Test {
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

TEST_F(OpenCartesianMeshTest, Basic) {
    ASSERT_EQ(10, mesh->numGrid(0, FULL));
    ASSERT_EQ(10, mesh->numGrid(0, FULL, true));
    ASSERT_EQ(11, mesh->numGrid(0, HALF));
    ASSERT_EQ(11, mesh->numGrid(0, HALF, true));
    ASSERT_EQ(10, mesh->numGrid(1, FULL));
    ASSERT_EQ(10, mesh->numGrid(1, FULL, true));
    ASSERT_EQ(11, mesh->numGrid(1, HALF));
    ASSERT_EQ(11, mesh->numGrid(1, HALF, true));
    ASSERT_EQ(10, mesh->numGrid(2, FULL));
    ASSERT_EQ(10, mesh->numGrid(2, FULL, true));
    ASSERT_EQ(11, mesh->numGrid(2, HALF));
    ASSERT_EQ(11, mesh->numGrid(2, HALF, true));
    ASSERT_EQ(100, mesh->totalNumGrid(CENTER, 2));
    ASSERT_EQ(1000, mesh->totalNumGrid(CENTER, 3));
    ASSERT_EQ(121, mesh->totalNumGrid(XY_VERTEX, 2));
    ASSERT_EQ(0, mesh->startIndex(0, FULL));
    ASSERT_EQ(9, mesh->endIndex(0, FULL));
    ASSERT_EQ(0, mesh->is(FULL));
    ASSERT_EQ(9, mesh->ie(FULL));
    ASSERT_EQ(0, mesh->startIndex(1, FULL));
    ASSERT_EQ(9, mesh->endIndex(1, FULL));
    ASSERT_EQ(0, mesh->js(FULL));
    ASSERT_EQ(9, mesh->je(FULL));
    ASSERT_EQ(0, mesh->startIndex(2, FULL));
    ASSERT_EQ(9, mesh->endIndex(2, FULL));
    ASSERT_EQ(0, mesh->ks(FULL));
    ASSERT_EQ(9, mesh->ke(FULL));
}

TEST_F(OpenCartesianMeshTest, IndexWrapping) {
    int l = 0;
    for (int k = mesh->ks(FULL); k <= mesh->ke(FULL); ++k) {
        for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
            for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
                ASSERT_EQ(l++, mesh->wrapIndex(CENTER, i, j, k));
            }
        }
    }
}

TEST_F(OpenCartesianMeshTest, Move) {
    SpaceCoord x0(3), x1(3);
    double dt = 0.1;
    Velocity v(3);
    CartesianMeshIndex idx(3);

    x0.setCoord(0.0, 0.0, 0.0);
    idx.locate(*mesh, x0);
    v(0) = 0.1;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x1(0)-0.01));
    ASSERT_GT(1.0e-15, fabs(x1(1)-0.0));
    ASSERT_GT(1.0e-15, fabs(x1(2)-0.0));
}

#endif // __Geomtk_OpenCartesainMesh_test__