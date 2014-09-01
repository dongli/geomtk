#ifndef __RLLMesh_test__
#define __RLLMesh_test__

#include "geomtk.h"

using namespace geomtk;

class RLLMeshTest : public ::testing::Test {
protected:
    const int FULL = RLLStagger::GridType::FULL;
    const int HALF = RLLStagger::GridType::HALF;
    const int CENTER = RLLStagger::Location::CENTER;
    const int X_FACE = RLLStagger::Location::X_FACE;
    const int Y_FACE = RLLStagger::Location::Y_FACE;
    const int XY_VERTEX = RLLStagger::Location::XY_VERTEX;
    const int Z_FACE = RLLStagger::Location::Z_FACE;

    SphereDomain *domain;
    RLLMesh *mesh;

    virtual void SetUp() {
        domain = new SphereDomain(CLASSIC_PRESSURE_SIGMA);
        mesh = new RLLMesh(*domain);
        mesh->init(10, 10, 5);
    }

    virtual void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLMeshTest, Basics) {
    ASSERT_EQ(10, mesh->getNumGrid(0, FULL));
    ASSERT_EQ(12, mesh->getNumGrid(0, FULL, true));
    ASSERT_EQ(10, mesh->getNumGrid(0, HALF));
    ASSERT_EQ(12, mesh->getNumGrid(0, HALF, true));
    ASSERT_EQ(10, mesh->getNumGrid(1, FULL));
    ASSERT_EQ(10, mesh->getNumGrid(1, FULL, true));
    ASSERT_EQ(9, mesh->getNumGrid(1, HALF));
    ASSERT_EQ(9, mesh->getNumGrid(1, HALF, true));
    ASSERT_EQ(5, mesh->getNumGrid(2, FULL));
    ASSERT_EQ(5, mesh->getNumGrid(2, FULL, true));
    ASSERT_EQ(6, mesh->getNumGrid(2, HALF));
    ASSERT_EQ(6, mesh->getNumGrid(2, HALF, true));
    ASSERT_EQ(-M_PI_2, mesh->getGridCoordComp(1, FULL, 0));
    ASSERT_EQ(M_PI_2, mesh->getGridCoordComp(1, FULL, mesh->getNumGrid(1, FULL)-1));
}

TEST_F(RLLMeshTest, IndexRanges) {
    ASSERT_EQ(1, mesh->getStartIndex(0, FULL));
    ASSERT_EQ(0, mesh->getStartIndex(1, FULL));
    ASSERT_EQ(1, mesh->getStartIndex(0, HALF));
    ASSERT_EQ(0, mesh->getStartIndex(1, HALF));
    ASSERT_EQ(10, mesh->getEndIndex(0, FULL));
    ASSERT_EQ(9,  mesh->getEndIndex(1, FULL));
    ASSERT_EQ(10, mesh->getEndIndex(0, HALF));
    ASSERT_EQ(8,  mesh->getEndIndex(1, HALF));
}

TEST_F(RLLMeshTest, IndexWrapping) {
    int l = 0;
    for (int k = mesh->ks(FULL); k <= mesh->ke(FULL); ++k) {
        for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
            for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
                ASSERT_EQ(l, mesh->wrapIndex(CENTER, i, j, k));
                int gridIdx[3];
                mesh->unwrapIndex(CENTER, l, gridIdx);
                ASSERT_EQ(i, gridIdx[0]);
                ASSERT_EQ(j, gridIdx[1]);
                ASSERT_EQ(k, gridIdx[2]);
                l++;
            }
        }
    }
}

TEST_F(RLLMeshTest, GridCoords) {
    int gridIdx[3];
    for (int loc = 0; loc < 5; ++loc) {
        ASSERT_EQ(mesh->getTotalNumGrid(loc, domain->getNumDim()),
                  mesh->gridCoords[loc].size());
        for (int i = 0; i < mesh->getTotalNumGrid(loc, 3); ++i) {
            const SphereCoord &x = mesh->getGridCoord(loc, i);
            mesh->unwrapIndex(loc, i, gridIdx);
            if (loc == CENTER || loc == Y_FACE || loc == Z_FACE) {
                ASSERT_EQ(mesh->getGridCoordComp(0, FULL, gridIdx[0]), x(0));
            } else if (loc == X_FACE || loc == XY_VERTEX) {
                ASSERT_EQ(mesh->getGridCoordComp(0, HALF, gridIdx[0]), x(0));
            }
            if (loc == CENTER || loc == X_FACE || loc == Z_FACE) {
                ASSERT_EQ(mesh->getGridCoordComp(1, FULL, gridIdx[1]), x(1));
            } else if (loc == Y_FACE || loc == XY_VERTEX) {
                ASSERT_EQ(mesh->getGridCoordComp(1, HALF, gridIdx[1]), x(1));
            }
            if (loc == CENTER || loc == X_FACE || loc == Y_FACE || loc == XY_VERTEX) {
                ASSERT_EQ(mesh->getGridCoordComp(2, FULL, gridIdx[2]), x(2));
            } else if (loc == Z_FACE) {
                ASSERT_EQ(mesh->getGridCoordComp(2, HALF, gridIdx[2]), x(2));
            }
            ASSERT_EQ(sin(x(0)), x.getSinLon());
            ASSERT_EQ(cos(x(0)), x.getCosLon());
            ASSERT_EQ(sin(x(1)), x.getSinLat());
            ASSERT_EQ(cos(x(1)), x.getCosLat());
        }
    }
}

TEST_F(RLLMeshTest, CosSinTan) {
    // check cosine and sine
    vec lonFull = mesh->getGridCoordComps(0, FULL, true);
    for (int i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
        ASSERT_EQ(cos(lonFull(i)), mesh->getCosLon(FULL, i));
        ASSERT_EQ(sin(lonFull(i)), mesh->getSinLon(FULL, i));
    }
    vec lonHalf = mesh->getGridCoordComps(0, HALF, true);
    for (int i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
        ASSERT_EQ(cos(lonHalf(i)), mesh->getCosLon(HALF, i));
        ASSERT_EQ(sin(lonHalf(i)), mesh->getSinLon(HALF, i));
    }
    vec latFull = mesh->getGridCoordComps(1, FULL, true);
    for (int j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        ASSERT_EQ(cos(latFull(j)), mesh->getCosLat(FULL, j));
        ASSERT_EQ(sin(latFull(j)), mesh->getSinLat(FULL, j));
        ASSERT_EQ(pow(sin(latFull(j)), 2), mesh->getSinLat2(FULL, j));
        ASSERT_EQ(tan(latFull(j)), mesh->getTanLat(FULL, j));
    }
    vec latHalf = mesh->getGridCoordComps(1, HALF, true);
    for (int j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        ASSERT_EQ(cos(latHalf(j)), mesh->getCosLat(HALF, j));
        ASSERT_EQ(sin(latHalf(j)), mesh->getSinLat(HALF, j));
        ASSERT_EQ(pow(sin(latHalf(j)), 2), mesh->getSinLat2(HALF, j));
        ASSERT_EQ(tan(latHalf(j)), mesh->getTanLat(HALF, j));
    }
}

TEST_F(RLLMeshTest, Move) {
    SphereCoord x0(3), x1(3);
    double dt = 0.1;
    SphereVelocity v(3);
    RLLMeshIndex idx(3);

    x0.setCoord(0.1, 0.0);
    x0(2) = 0.5;
    idx.locate(*mesh, x0);
    v(0) = 0.1*M_PI;
    v(1) = 0.0;
    v(2) = 0.0;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_EQ(x0(0)+v(0)*dt, x1(0));
    ASSERT_EQ(x0(1), x1(1));

    v(0) = PI2;
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x0(0)+v(0)*dt-x1(0)));
    ASSERT_EQ(x0(1), x1(1));

    x0.setCoordComp(1, 89.5*RAD);
    v(0) = 0.0;
    v(1) = 0.0;
    v.transformToPS(x0);
    idx.reset();
    idx.locate(*mesh, x0);
    x0.transformToPS(*domain);
    mesh->move(x0, dt, v, idx, x1);
    ASSERT_GT(1.0e-15, fabs(x0(0)-x1(0)));
    ASSERT_GT(1.0e-15, fabs(x0(1)-x1(1)));

    v(0) = 1.0*RAD;
    v.transformToPS(x0);
    mesh->move(x0, dt, v, idx, x1);
    // TODO: How to test?
}

TEST_F(RLLMeshTest, CellVolume) {
    mesh->setCellVolumes();
    double totalVolume = 0.0;
    for (int i = 0; i < mesh->getTotalNumGrid(CENTER, 2); ++i) {
        totalVolume += mesh->getCellVolume(i);
    }
    ASSERT_GT(1.0e-13, fabs(4*M_PI*domain->getRadius()*domain->getRadius()-totalVolume));
}

#endif
