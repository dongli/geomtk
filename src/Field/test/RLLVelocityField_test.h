#ifndef __RLLVelocityField_test__
#define __RLLVelocityField_test__

#include "RLLVelocityField.h"
#include "RLLMesh_test.h"

class RLLVelocityFieldTest : public RLLMeshTest {
protected:
    RLLVelocityField *field;

    virtual void SetUp() {
        RLLMeshTest::SetUp();
        field = new RLLVelocityField(*mesh);
    }

    virtual void TearDown() {
        delete field;
    }
};

TEST_F(RLLVelocityFieldTest, CreateAndSet) {
    field->create(EDGE,   CENTER, CENTER,
                  CENTER, EDGE,   CENTER,
                  CENTER, CENTER, EDGE);
    ASSERT_EQ(12, field->rings[0].data[0].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[0].data[0].get(0).n_cols);
    ASSERT_EQ(12, field->rings[0].data[1].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[0].data[1].get(0).n_cols);
    ASSERT_EQ(12, field->rings[0].data[2].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[0].data[2].get(0).n_cols);
    ASSERT_EQ(12, field->rings[1].data[0].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[1].data[0].get(0).n_cols);
    ASSERT_EQ(12, field->rings[1].data[1].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[1].data[1].get(0).n_cols);
    ASSERT_EQ(12, field->rings[1].data[2].get(0).n_rows);
    ASSERT_EQ(5,  field->rings[1].data[2].get(0).n_cols);
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            for (int i = 0; i < mesh->getNumGrid(0, EDGE); ++i) {
                (*field)(0, 0, i, j, k) = i+j*mesh->getNumGrid(0, EDGE);
            }
        }
    }
    field->applyBndCond(0);
    // check boundary grids
    int n = mesh->getNumGrid(0, CENTER, true);
    for (int l = 0; l < 2; ++l) {
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            ASSERT_EQ(field->rings[l].data[0].get(0)(0, k), field->rings[l].data[0].get(0)(n-2, k));
            ASSERT_EQ(field->rings[l].data[0].get(0)(n-1, k), field->rings[l].data[0].get(0)(1, k));
        }
    }
    // check rings
    for (int l = 0; l < 2; ++l) {
        int j = l == 0 ? 1 : mesh->getNumGrid(1, CENTER)-2;
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            for (int i = 1; i < mesh->getNumGrid(0, CENTER, true)-1; ++i) {
                ASSERT_EQ(field->rings[l].data[0].get(0)(i, k),
                    (field->data[0].get(0)(i, j, k)+field->data[0].get(0)(i-1, j, k))*0.5);
            }
        }
    }
}

#endif
