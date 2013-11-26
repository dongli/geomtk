#include "Mesh.h"
#include "SphereDomain.h"
#include "gtest/gtest.h"

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
