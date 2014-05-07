#ifndef __Geomtk_ConfigManager_test__
#define __Geomtk_ConfigManager_test__

#include "geomtk.h"

using namespace geomtk;

class ConfigManagerTest : public ::testing::Test {
protected:
    ConfigManager configManager;

    void SetUp() {

    }

    void TearDown() {

    }
};

TEST_F(ConfigManagerTest, Parse) {
    configManager.parse("/Users/dongli/Dropbox/Works/geodiag/tools/geomtk/src/Utils/test/test.config");

    double a;
    configManager.getValue("test_config", "a", a);
    ASSERT_EQ(a, 1.0);
    string b;
    configManager.getValue("test_config", "b", b);
    ASSERT_EQ(b, "hello, \\\"world\\\"! It's me.");
    double c;
    configManager.getValue("test_config", "c", c);
    ASSERT_EQ(c, -1.0);
    double d;
    configManager.getValue("test_config", "d", d);
    ASSERT_EQ(d, -1.0e+10);
}

#endif