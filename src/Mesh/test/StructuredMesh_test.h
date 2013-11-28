#ifndef __StructuredMesh_test__
#define __StructuredMesh_test__

#include "SphereDomain.h"
#include "StructuredMesh.h"

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

TEST_F(StructuredMeshTest, SetGridCoords) {
    // -------------------------------------------------------------------------
    int numLon = 10;
    double fullLon[numLon], halfLon[numLon];
    double dlon = 2.0*M_PI/numLon;
    for (int i = 0; i < numLon; ++i) {
        fullLon[i] = i*dlon;
        halfLon[i] = i*dlon+dlon*0.5;
    }
    mesh->setGridCoords(0, numLon, fullLon, halfLon);
    EXPECT_EQ(numLon+2, mesh->getNumGrid(0, CENTER, true));
    EXPECT_EQ(numLon, mesh->getNumGrid(0, CENTER));
    vec lon1 = mesh->getGridCoords(0, CENTER);
    EXPECT_EQ(numLon, lon1.size());
    EXPECT_EQ(fullLon[0], lon1(0));
    EXPECT_EQ(fullLon[numLon-1], lon1(numLon-1));
    vec lon2 = mesh->getGridCoords(0, EDGE);
    EXPECT_EQ(numLon, lon2.size());
    EXPECT_EQ(halfLon[0], lon2(0));
    EXPECT_EQ(halfLon[numLon-1], lon2(numLon-1));
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
    EXPECT_EQ(numLat, lat1.size());
    EXPECT_EQ(mesh->getDomain().getAxisStart(1), lat1(0));
    EXPECT_EQ(mesh->getDomain().getAxisEnd(1), lat1(numLat-1));
    vec lat2 = mesh->getGridCoords(1, EDGE);
    EXPECT_EQ(numLat-1, lat2.size());
    EXPECT_EQ(lat1(0)+dlat*0.5, lat2(0));
    // NOTE: The equality is not exact!
    EXPECT_GT(1.0e-15, std::abs(lat1(numLat-1)-dlat*0.5-lat2(numLat-2)));
}

#endif
