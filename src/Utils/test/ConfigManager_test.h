#ifndef __GEOMTK_ConfigManager_test__
#define __GEOMTK_ConfigManager_test__

#include "geomtk.h"

using namespace geomtk;

TEST(ConfigManager, Parse) {
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

    ConfigManager::parse("test.config");

    double a = ConfigManager::getValue<double>("test_config", "a");
    ASSERT_EQ(a, 1.0);
    string b = ConfigManager::getValue<string>("test_config", "b");
    ASSERT_EQ(b, "hello, \"world\"!");
    double c = ConfigManager::getValue<double>("test_config", "c");
    ASSERT_EQ(c, -1.0);
    double d = ConfigManager::getValue<double>("test_config", "d");
    ASSERT_EQ(d, -1.0e+10);

    ASSERT_TRUE(ConfigManager::hasGroup("test_config"));
    ASSERT_FALSE(ConfigManager::hasGroup("foo"));

    SystemTools::removeFile("test.config");
}

TEST(ConfigManager, AddKeyValue) {
    ConfigManager::addKeyValue("test_config", "e", "abc");
    ASSERT_EQ("abc", ConfigManager::getValue<string>("test_config", "e"));
}

#endif // __GEOMTK_ConfigManager_test__