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
    stringstream ss;
    ss << "{" << endl;
    ss << "  \"test_config\":" << endl;
    ss << "  {" << endl;
    ss << "    \"a\": 1.0," << endl;
    ss << "    \"b\": \"hello, \\\"world\\\"!\"," << endl;
    ss << "    \"c\": -1," << endl;
    ss << "    \"d\": -1.0e+10" << endl;
    ss << "  }" << endl;
    ss << "}" << endl;
    SystemTools::writeFile("test.config", ss.str());

    configManager.parse("test.config");

    double a = configManager.getValue<double>("test_config", "a");
    ASSERT_EQ(a, 1.0);
    string b = configManager.getValue<string>("test_config", "b");
    ASSERT_EQ(b, "hello, \"world\"!");
    double c = configManager.getValue<double>("test_config", "c");
    ASSERT_EQ(c, -1.0);
    double d = configManager.getValue<double>("test_config", "d");
    ASSERT_EQ(d, -1.0e+10);

    SystemTools::removeFile("test.config");
}

#endif