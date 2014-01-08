#ifndef __StructuredMesh_test__
#define __StructuredMesh_test__

#include "SphereDomain.h"
#include "StructuredMesh.h"

using namespace geomtk;

class StructuredMeshTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    StructuredMesh *mesh;

    virtual void SetUp() {
        sphere = new SphereDomain(2);
        mesh = new StructuredMesh(*sphere);
    }

    virtual void TearDown() {
        delete sphere;
        delete mesh;
    }
};

TEST_F(StructuredMeshTest, SetEquidistantGrids) {
    // -------------------------------------------------------------------------
    int numLon = 10;
    double fullLon[numLon], halfLon[numLon];
    double dlon = 2.0*M_PI/numLon;
    for (int i = 0; i < numLon; ++i) {
        fullLon[i] = i*dlon;
        halfLon[i] = i*dlon+dlon*0.5;
    }
    mesh->setGridCoords(0, numLon, fullLon, halfLon);
    ASSERT_EQ(numLon+2, mesh->getNumGrid(0, CENTER, true));
    ASSERT_EQ(numLon, mesh->getNumGrid(0, CENTER));
    vec lon1 = mesh->getGridCoords(0, CENTER);
    ASSERT_EQ(numLon, lon1.size());
    ASSERT_EQ(fullLon[0], lon1(0));
    ASSERT_EQ(fullLon[numLon-1], lon1(numLon-1));
    vec lon2 = mesh->getGridCoords(0, EDGE);
    ASSERT_EQ(numLon, lon2.size());
    ASSERT_EQ(halfLon[0], lon2(0));
    ASSERT_EQ(halfLon[numLon-1], lon2(numLon-1));
    ASSERT_EQ(true, mesh->isEquidistant(0));
    // -------------------------------------------------------------------------
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
    vec lat1 = mesh->getGridCoords(1, CENTER);
    ASSERT_EQ(numLat, lat1.size());
    ASSERT_EQ(mesh->getDomain().getAxisStart(1), lat1(0));
    ASSERT_EQ(mesh->getDomain().getAxisEnd(1), lat1(numLat-1));
    vec lat2 = mesh->getGridCoords(1, EDGE);
    ASSERT_EQ(numLat-1, lat2.size());
    ASSERT_EQ(lat1(0)+dlat*0.5, lat2(0));
    // NOTE: The equality is not exact!
    EXPECT_GT(1.0e-15, fabs(lat1(numLat-1)-dlat*0.5-lat2(numLat-2)));
    ASSERT_EQ(true, mesh->isEquidistant(0));
    ASSERT_EQ(false, mesh->isEquidistant(1));
}

TEST_F(StructuredMeshTest, SetInequidistantGrids) {
    const int numLon = 5;
    double fullLon[numLon] = {0.0, 30*RAD, 45*RAD, 90*RAD, 130*RAD};
    double halfLon[numLon] = {13*RAD, 40*RAD, 81*RAD, 102*RAD, 150*RAD};
    mesh->setGridCoords(0, numLon, fullLon, halfLon);
    ASSERT_EQ(7, mesh->getNumGrid(0, CENTER, true));
    ASSERT_EQ(7, mesh->getNumGrid(0, EDGE, true));
    ASSERT_EQ(-230.0*RAD, mesh->getGridCoord(0, CENTER, -1));
    ASSERT_GT(1.0e-15, fabs(-210.0*RAD-mesh->getGridCoord(0, EDGE, -1)));
//    ASSERT_GT(1.0e-15, fabs(230.0*RAD-mesh->getGridInterval(0, EDGE, -1)));
}

#endif
