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

TEST_F(StructuredMeshTest, SetCoords) {
    int numLon = 10;
    double fullLon[numLon], halfLon[numLon];
    double dlon = 2.0*M_PI/numLon;
    for (int i = 0; i < numLon; ++i) {
        fullLon[i] = i*dlon;
        halfLon[i] = i*dlon+dlon*0.5;
    }
    mesh->setCoords(0, numLon, fullLon, halfLon);
    const vec &lon1 = mesh->getCoords(0, CENTER);
    EXPECT_EQ(numLon+2, lon1.size());
    EXPECT_EQ(fullLon[0]-dlon, lon1(0));
    EXPECT_EQ(fullLon[numLon-1]+dlon, lon1(numLon+1));
    const vec &lon2 = mesh->getCoords(0, EDGE);
    EXPECT_EQ(numLon+2, lon2.size());
    EXPECT_EQ(halfLon[0]-dlon, lon2(0));
    EXPECT_EQ(halfLon[numLon-1]+dlon, lon2(numLon+1));
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
    mesh->setCoords(1, numLat, fullLat, halfLat);
    const vec &lat1 = mesh->getCoords(1, CENTER);
    EXPECT_EQ(numLat, lat1.size());
    EXPECT_EQ(mesh->getDomain().getAxisStart(1), lat1(0));
    EXPECT_EQ(mesh->getDomain().getAxisEnd(1), lat1(numLat-1));
    const vec &lat2 = mesh->getCoords(1, EDGE);
    EXPECT_EQ(numLat-1, lat2.size());
    EXPECT_EQ(lat1(0)+dlat*0.5, lat2(0));
    // NOTE: The equality is not exact!
    EXPECT_GT(1.0e-15, std::abs(lat1(numLat-1)-dlat*0.5-lat2(numLat-2)));
}
