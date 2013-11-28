#include "gtest/gtest.h"
#include "Domain/test/Domain_test.h"
#include "Domain/test/SphereDomain_test.h"
#include "Mesh/test/Mesh_test.h"
#include "Mesh/test/StructuredMesh_test.h"
#include "Field/test/RLLScalarField_test.h"
#include "Field/test/RLLVectorField_test.h"
#include "Field/test/RLLVelocityField_test.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
