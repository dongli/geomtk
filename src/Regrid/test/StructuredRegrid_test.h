#include "StructuredRegrid.h"

class StructuredRegridTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    RLLScalarField *field;
    StructuredRegrid *regrid;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        field = new RLLScalarField(*mesh);
        regrid = new StructuredRegrid(*mesh);

        int numLon = 10;
        double fullLon[numLon], halfLon[numLon];
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
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

        field->create(CENTER, CENTER);
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
                (*field)(0, i, j) = i+j*mesh->getNumGrid(0, CENTER);
            }
        }
    }

    virtual void TearDown() {
        delete regrid;
        delete field;
        delete mesh;
        delete domain;
    }
};

TEST_F(StructuredRegridTest, Run) {
    SpaceCoord x(2);
    double y;

    regrid->run(*field, x, y);
}
