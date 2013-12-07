#ifndef __Mesh_test__
#define __Mesh_test__

#include "Mesh.h"
#include "SphereDomain.h"

using namespace geomtk;

class MeshTest : public ::testing::Test {
protected:
    SphereDomain *sphere;
    Mesh *mesh;

    virtual void SetUp() {
        sphere = new SphereDomain;
        mesh = new Mesh(*sphere);
    }

    virtual void TearDown() {

    }
};

#endif
