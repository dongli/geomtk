#ifndef __RLLVelocityField_test__
#define __RLLVelocityField_test__

#include "RLLVelocityField.h"
#include "RLLMesh_test.h"

class RLLVelocityFieldTest : public RLLMeshTest {
protected:
    RLLVelocityField *v;

    virtual void SetUp() {
        RLLMeshTest::SetUp();
        v = new RLLVelocityField(*mesh);
    }

    virtual void TearDown() {
        delete v;
    }
};

TEST_F(RLLVelocityFieldTest, CreateAndSet) {
    v->create(EDGE,   CENTER, CENTER,
              CENTER, EDGE,   CENTER,
              CENTER, CENTER, EDGE);
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
                (*v)(0, 0, i, j, k) = i+j*mesh->getNumGrid(0, EDGE);
            }
        }
    }
    v->applyBndCond(0);
    // check boundary grids
    int n = mesh->getNumGrid(0, CENTER);
    for (int m = 0; m < domain->getNumDim(); ++m) {
        for (int l = 0; l < 2; ++l) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                ASSERT_EQ(v->rings[l].getOriginalData(0, m, -1, k), v->rings[l].getOriginalData(0, m, n-1, k));
                ASSERT_EQ(v->rings[l].getOriginalData(0, m,  n, k), v->rings[l].getOriginalData(0, m, 0, k));
                ASSERT_EQ(v->rings[l].getTransformedData(0, m, -1, k), v->rings[l].getTransformedData(0, m, n-1, k));
                ASSERT_EQ(v->rings[l].getTransformedData(0, m,  n, k), v->rings[l].getTransformedData(0, m, 0, k));
            }
        }
    }
    // check rings
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
                ASSERT_EQ(v->rings[l].getOriginalData(0, 0, i, k), ((*v)(0, 0, i, j, k)+(*v)(0, 0, i-1, j, k))*0.5);
                ASSERT_EQ(v->rings[l].getOriginalData(0, 1, j, k), ((*v)(0, 1, i, j, k)+(*v)(0, 1, j+1, j, k))*0.5);
                ASSERT_EQ(v->rings[l].getOriginalData(0, 2, j, k), ((*v)(0, 2, i, j, k)+(*v)(0, 2, k+1, j, k))*0.5);
            }
        }
    }
}

#endif
