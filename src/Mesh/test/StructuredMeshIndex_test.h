#ifndef __StructuredMeshIndex_test__
#define __StructuredMeshIndex_test__

#include "StructuredMeshIndex.h"
#include "SphereDomain.h"

using namespace geomtk;

class StructuredMeshIndexTest : public ::testing::Test {
protected:
    SphereDomain *domain;
    StructuredMesh *mesh;
    StructuredMeshIndex *index;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        mesh = new StructuredMesh(*domain);
        index = new StructuredMeshIndex(domain->getNumDim());

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
    }

    virtual void TearDown() {
        delete index;
        delete mesh;
        delete domain;
    }
};

//     \      -      |      +      |      +      |      +      |      +      |      +      \      -
// -0.4*PI          0.0         0.4*PI        0.8*PI        1.2*PI        1.6*PI        2.0*PI
//        -0.2*PI        0.2*PI        0.6*PI        1.0*PI        1.4*PI        1.8*PI        2.2*PI
//     |      +      |      +      |      +      |      +      |
// -0.5*PI      -0.25*PI          0.0        0.25*PI        0.5*PI
//      -0.375*PI     -0.125*PI      0.125*PI      0.375*PI

TEST_F(StructuredMeshIndexTest, Locate) {
    SphereCoord x(domain->getNumDim());

    x(0) =  0.9*M_PI;
    x(1) = -0.11*M_PI;
    index->locate(*mesh, x);
    ASSERT_EQ(2, (*index)(0, CENTER));
    ASSERT_EQ(1, (*index)(1, CENTER));
    ASSERT_EQ(1, (*index)(0, EDGE));
    ASSERT_EQ(1, (*index)(1, EDGE));

    x(0) = 0.14*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(0, (*index)(0, CENTER));
    ASSERT_EQ(-1, (*index)(0, EDGE));

    x(0) = 1.9*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(4, (*index)(0, CENTER));
    ASSERT_EQ(4, (*index)(0, EDGE));

    x(1) = -0.4*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(0, (*index)(1, CENTER));
    ASSERT_EQ(-1, (*index)(1, EDGE));

    x(1) = 0.41*M_PI;
    index->reset();
    index->locate(*mesh, x);
    ASSERT_EQ(3, (*index)(1, CENTER));
    ASSERT_EQ(3, (*index)(1, EDGE));
}

TEST_F(StructuredMeshIndexTest, AssignmentOperator) {
    StructuredMeshIndex a(3), b(3);

    a.indices[0][CENTER] = 1;
    a.indices[0][EDGE] = 2;
    a.indices[0][VERTEX] = 3;
    a.indices[1][CENTER] = 4;
    a.indices[1][EDGE] = 5;
    a.indices[1][VERTEX] = 6;
    a.indices[2][CENTER] = 7;
    a.indices[2][EDGE] = 8;
    a.indices[2][VERTEX] = 9;

    b = a;

    ASSERT_EQ(a.indices[0][CENTER], b.indices[0][CENTER]);
    ASSERT_EQ(a.indices[0][EDGE],   b.indices[0][EDGE]);
    ASSERT_EQ(a.indices[0][VERTEX], b.indices[0][VERTEX]);
    ASSERT_EQ(a.indices[1][CENTER], b.indices[1][CENTER]);
    ASSERT_EQ(a.indices[1][EDGE],   b.indices[1][EDGE]);
    ASSERT_EQ(a.indices[1][VERTEX], b.indices[1][VERTEX]);
    ASSERT_EQ(a.indices[2][CENTER], b.indices[2][CENTER]);
    ASSERT_EQ(a.indices[2][EDGE],   b.indices[2][EDGE]);
    ASSERT_EQ(a.indices[2][VERTEX], b.indices[2][VERTEX]);
}

#endif
