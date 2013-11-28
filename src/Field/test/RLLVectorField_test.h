#ifndef __RLLVectorField_test__
#define __RLLVectorField_test__

#include "RLLVectorField.h"
#include "RLLMesh_test.h"

class RLLVectorFieldTest : public RLLMeshTest {
protected:
    RLLVectorField *field;

    virtual void SetUp() {
        RLLMeshTest::SetUp();
        field = new RLLVectorField(*mesh);
    }

    virtual void TearDown() {
        delete field;
    }
};

TEST_F(RLLVectorFieldTest, CreateAndSet) {
    field->create(EDGE,   CENTER, CENTER,
                  CENTER, EDGE,   CENTER,
                  CENTER, CENTER, EDGE);
    ASSERT_EQ(12, field->data[0].get(0).n_rows);
    ASSERT_EQ(10, field->data[0].get(0).n_cols);
    ASSERT_EQ(5,  field->data[0].get(0).n_slices);
    ASSERT_EQ(12, field->data[1].get(0).n_rows);
    ASSERT_EQ(9,  field->data[1].get(0).n_cols);
    ASSERT_EQ(5,  field->data[1].get(0).n_slices);
    ASSERT_EQ(12, field->data[2].get(0).n_rows);
    ASSERT_EQ(10, field->data[2].get(0).n_cols);
    ASSERT_EQ(6,  field->data[2].get(0).n_slices);
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
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            ASSERT_EQ(field->data[0].get(0)(0, j, k), field->data[0].get(0)(n-2, j, k));
            ASSERT_EQ(field->data[0].get(0)(n-1, j, k), field->data[0].get(0)(1, j, k));
        }
    }
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, EDGE); ++j) {
            ASSERT_EQ(field->data[1].get(0)(0, j, k), field->data[1].get(0)(n-2, j, k));
            ASSERT_EQ(field->data[1].get(0)(n-1, j, k), field->data[1].get(0)(1, j, k));
        }
    }
    for (int k = 0; k < mesh->getNumGrid(2, EDGE); ++k) {
        for (int j = 0; j < mesh->getNumGrid(1, CENTER); ++j) {
            ASSERT_EQ(field->data[2].get(0)(0, j, k), field->data[2].get(0)(n-2, j, k));
            ASSERT_EQ(field->data[2].get(0)(n-1, j, k), field->data[2].get(0)(1, j, k));
        }
    }
}

#endif
