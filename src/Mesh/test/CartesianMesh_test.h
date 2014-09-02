#ifndef __Geomtk_CartesainMesh_test__
#define __Geomtk_CartesainMesh_test__

#include "geomtk.h"

using namespace geomtk;

class CartesianMeshTest : public ::testing::Test {
protected:
    const int FULL = StructuredStagger::GridType::FULL;
    const int HALF = StructuredStagger::GridType::HALF;
    const int CENTER = StructuredStagger::Location::CENTER;

    CartesianDomain *domain;
    CartesianMesh *mesh;

    virtual void SetUp() {
        domain = new CartesianDomain(3);
        mesh = new CartesianMesh(*domain);

        domain->setAxis(0, "x", "x axis", "m", 0, PERIODIC, 1, PERIODIC);
        domain->setAxis(1, "y", "y axis", "m", 0, PERIODIC, 1, PERIODIC);
        domain->setAxis(2, "z", "z axis", "m", 0, PERIODIC, 1, PERIODIC);

        int nx = 10, ny = 10, nz = 10;
        double dx = 1.0/nx, dy = 1.0/ny, dz = 1.0/nz;
        vec x(nx), y(ny), z(nz);
        for (int i = 0; i < nx; ++i) {
            x[i] = i*dx;
        }
        for (int j = 0; j < ny; ++j) {
            y[j] = j*dy;
        }
        for (int k = 0; k < nz; ++k) {
            z[k] = k*dz;
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

TEST_F(CartesianMeshTest, Basic) {
    ASSERT_EQ(10, mesh->getNumGrid(0, FULL));
    ASSERT_EQ(12, mesh->getNumGrid(0, FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(0, HALF));
    ASSERT_EQ(12, mesh->getNumGrid(0, HALF, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, FULL));
    ASSERT_EQ(12, mesh->getNumGrid(1, FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, HALF));
    ASSERT_EQ(12, mesh->getNumGrid(1, HALF, true));
    ASSERT_EQ(10, mesh->getNumGrid(2, FULL));
    ASSERT_EQ(12, mesh->getNumGrid(2, FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(2, HALF));
    ASSERT_EQ(12, mesh->getNumGrid(2, HALF, true));
    ASSERT_EQ(100, mesh->getTotalNumGrid(CENTER, 2));
    ASSERT_EQ(1000, mesh->getTotalNumGrid(CENTER, 3));
    ASSERT_EQ(1, mesh->getStartIndex(0, FULL));
    ASSERT_EQ(10, mesh->getEndIndex(0, FULL));
    ASSERT_EQ(1, mesh->is(FULL));
    ASSERT_EQ(10, mesh->ie(FULL));
    ASSERT_EQ(1, mesh->getStartIndex(1, FULL));
    ASSERT_EQ(10, mesh->getEndIndex(1, FULL));
    ASSERT_EQ(1, mesh->js(FULL));
    ASSERT_EQ(10, mesh->je(FULL));
    ASSERT_EQ(1, mesh->getStartIndex(2, FULL));
    ASSERT_EQ(10, mesh->getEndIndex(2, FULL));
    ASSERT_EQ(1, mesh->ks(FULL));
    ASSERT_EQ(10, mesh->ke(FULL));
}

TEST_F(CartesianMeshTest, IndexWrapping) {
    int l = 0;
    for (int k = mesh->ks(FULL); k <= mesh->ke(FULL); ++k) {
        for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
            for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
                ASSERT_EQ(l++, mesh->wrapIndex(CENTER, i, j, k));
            }
        }
    }
}

TEST_F(CartesianMeshTest, Move) {
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

    v(0) = 11.0;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x1(0)-0.1));
    ASSERT_GT(1.0e-15, fabs(x1(1)-0.0));
    ASSERT_GT(1.0e-15, fabs(x1(2)-0.0));
}

#endif // __Geomtk_CartesainMesh_test__