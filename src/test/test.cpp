#include "gtest/gtest.h"
#include "TimeLevels_test.h"
#include "TimeManager_test.h"
#include "SpaceCoord_test.h"
#include "CartesianDomain_test.h"
#include "SphereCoord_test.h"
#include "SphereDomain_test.h"
#include "PeriodicCartesianMesh_test.h"
#include "OpenCartesianMesh_test.h"
#include "OpenCartesianMeshIndex_test.h"
#include "RLLMesh_test.h"
#include "RLLMeshIndex_test.h"
#include "RLLField_test.h"
#include "RLLVelocityField_test.h"
#include "RLLRegrid_test.h"
#include "IOManager_test.h"
#include "ConfigManager_test.h"
#include "StampString_test.h"
#include "Numerics_test.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
