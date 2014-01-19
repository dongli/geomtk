#ifndef __StructuredMesh_test__
#define __StructuredMesh_test__

#include "SphereDomain.h"
#include "StructuredMesh.h"

using namespace geomtk;

class StructuredMeshTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    StructuredMesh *mesh;

    int numLon, numLat;
    double dlon, dlat;
    double *fullLon, *halfLon;
    double *fullLat, *halfLat;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new StructuredMesh(*domain);

        numLon = 10;
        fullLon = new double[numLon];
        halfLon = new double[numLon];
        dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        numLat = 10;
        fullLat = new double[numLat];
        halfLat = new double[numLat-1];
        dlat = M_PI/(numLat-1);
        for (int j = 0; j < numLat; ++j) {
            fullLat[j] = j*dlat-M_PI_2;
        }
        for (int j = 0; j < numLat-1; ++j) {
            halfLat[j] = dlat*0.5+j*dlat-M_PI_2;
        }
        mesh->setGridCoords(1, numLat, fullLat, halfLat);
    }

    virtual void TearDown() {
        delete domain;
        delete mesh;
        delete [] fullLon;
        delete [] halfLon;
        delete [] fullLat;
        delete [] halfLat;
    }
};

TEST_F(StructuredMeshTest, CheckGridDimensions) {
    ASSERT_EQ(numLon+2, mesh->getNumGrid(0, CENTER, true));
    ASSERT_EQ(numLon, mesh->getNumGrid(0, CENTER));
    ASSERT_EQ(numLon+2, mesh->getNumGrid(0, EDGE, true));
    ASSERT_EQ(numLon, mesh->getNumGrid(0, EDGE));
    ASSERT_EQ(numLat, mesh->getNumGrid(1, CENTER));
    ASSERT_EQ(numLat-1, mesh->getNumGrid(1, EDGE));
}

TEST_F(StructuredMeshTest, CheckGridCoords) {
    vec lon1 = mesh->getGridCoords(0, CENTER);
    ASSERT_EQ(numLon, lon1.size());
    ASSERT_EQ(fullLon[0], lon1(0));
    ASSERT_EQ(fullLon[numLon-1], lon1(numLon-1));
    vec lon2 = mesh->getGridCoords(0, EDGE);
    ASSERT_EQ(numLon, lon2.size());
    ASSERT_EQ(halfLon[0], lon2(0));
    ASSERT_EQ(halfLon[numLon-1], lon2(numLon-1));
    ASSERT_EQ(true, mesh->isEquidistant(0));

    vec lat1 = mesh->getGridCoords(1, CENTER);
    ASSERT_EQ(numLat, lat1.size());
    ASSERT_EQ(mesh->getDomain().getAxisStart(1), lat1(0));
    ASSERT_EQ(mesh->getDomain().getAxisEnd(1), lat1(numLat-1));
    vec lat2 = mesh->getGridCoords(1, EDGE);
    ASSERT_EQ(numLat-1, lat2.size());
    ASSERT_EQ(lat1(0)+dlat*0.5, lat2(0));
    // NOTE: The equality is not exact!
    ASSERT_GT(1.0e-15, fabs(lat1(numLat-1)-dlat*0.5-lat2(numLat-2)));
    ASSERT_EQ(true, mesh->isEquidistant(0));
    ASSERT_EQ(false, mesh->isEquidistant(1));
}

TEST_F(StructuredMeshTest, UnwrapIndex) {
    int i[3];
    mesh->unwrapIndex(0, i, A_GRID);
    ASSERT_EQ(0, i[0]);
    ASSERT_EQ(0, i[1]);
    ASSERT_EQ(0, i[2]);
    mesh->unwrapIndex(5, i, A_GRID);
    ASSERT_EQ(5, i[0]);
    ASSERT_EQ(0, i[1]);
    ASSERT_EQ(0, i[2]);
    mesh->unwrapIndex(9, i, A_GRID);
    ASSERT_EQ(9, i[0]);
    ASSERT_EQ(0, i[1]);
    ASSERT_EQ(0, i[2]);
    mesh->unwrapIndex(10, i, A_GRID);
    ASSERT_EQ(0, i[0]);
    ASSERT_EQ(1, i[1]);
    ASSERT_EQ(0, i[2]);
    mesh->unwrapIndex(99, i, A_GRID);
    ASSERT_EQ(9, i[0]);
    ASSERT_EQ(9, i[1]);
    ASSERT_EQ(0, i[2]);
}

TEST_F(StructuredMeshTest, GetGridCoord) {
    SpaceCoord x(domain->getNumDim());
    int i[3];
    int cellIdx[10] = {0,13,31,34,90,32,43,83,99,50};
    for (int n = 0; n < 10; ++n) {
        mesh->getGridCoord(cellIdx[n], x, A_GRID);
        mesh->unwrapIndex(cellIdx[n], i, A_GRID);
        for (int m = 0; m < domain->getNumDim(); ++m) {
            ASSERT_EQ(mesh->getGridCoordComp(m, CENTER, i[m]), x(m));
        }
    }
}

#endif
