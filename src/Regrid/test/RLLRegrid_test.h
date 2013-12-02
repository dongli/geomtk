#include "RLLRegrid.h"

class RLLRegridTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    RLLMesh *mesh;
    RLLRegrid *regrid;
    RLLScalarField *q;
    RLLVelocityField *v;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        regrid = new RLLRegrid(*mesh);
        q = new RLLScalarField(*mesh);
        v = new RLLVelocityField(*mesh);

        domain->setRadius(1.0);

        int numLon = 5;
        double fullLon[numLon], halfLon[numLon];
        double dlon = 2.0*M_PI/numLon;
        for (int i = 0; i < numLon; ++i) {
            fullLon[i] = i*dlon;
            halfLon[i] = i*dlon+dlon*0.5;
        }
        mesh->setGridCoords(0, numLon, fullLon, halfLon);
        int numLat = 5;
        double fullLat[numLat], halfLat[numLat-1];
        double dlat = M_PI/(numLat-1);
        for (int j = 0; j < numLat; ++j) {
            fullLat[j] = j*dlat-M_PI_2;
        }
        for (int j = 0; j < numLat-1; ++j) {
            halfLat[j] = dlat*0.5+j*dlat-M_PI_2;
        }
        mesh->setGridCoords(1, numLat, fullLat, halfLat);

        v->create(EDGE, CENTER, CENTER, EDGE);
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
                (*v)(0, 0, i, j) = i+j*mesh->getNumGrid(0, CENTER);
                (*v)(1, 0, i, j) = i+j*mesh->getNumGrid(0, CENTER);
            }
        }
        v->applyBndCond(0);
    }

    virtual void TearDown() {
        delete q;
        delete regrid;
        delete mesh;
        delete domain;
    }
};

TEST_F(RLLRegridTest, Run) {
    SphereCoord x(2);
    double y;

    x(0) = 1.9*M_PI;
    x(1) = 0.2*M_PI;

    SphereVelocity z(2);
    regrid->run(BILINEAR, 0, *v, x, z);
    
    x(0) = 0.41*M_PI;
    regrid->run(BILINEAR, 0, *v, x, z);
}
