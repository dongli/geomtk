#ifndef __GEOMTK_ConfigManager__
#define __GEOMTK_ConfigManager__

#include "geomtk_commons.h"

namespace geomtk {

class ConfigManager {
    static map<string, ptree> ptrees;
public:
    /**
     *  Parse the configuration from a given file.
     *
     *  @param filePath the configure file path.
     *  @param mute     the control boolean.
     */
    static void
    parse(const string &filePath, bool mute = false);

    template <typename T>
    static void
    addKeyValue(const string &group, const string &key, const T &value);

    static bool
    hasGroup(const string &group);

    static vector<string>
    getGroups();

    static bool
    hasKey(const string &group, const string &key);

    static vector<string>
    getKeys(const string &group);

    template <typename T>
    static T
    getValue(const string &group, const string &key);

    template <typename T>
    static T
    getValue(const string &group, const string &key, const T &defaultValue);

    static void
    print();
private:
    static void
    printPropertyTree(const boost::property_tree::ptree &pt, int level = 0);
}; // ConfigManager

template <typename T>
void ConfigManager::
addKeyValue(const string &group, const string &key, const T &value) {
    if (hasKey(group, key)) {
        REPORT_ERROR("Already add key \"" << key << "\" in group \"" <<
                     group << "\"!");
    }
    if (ptrees.find("memory") != ptrees.end()) {
        ptree pt;
        ptrees["memory"] = pt;
    }
    ptrees["memory"].put(group+"."+key, value);
} // addKeyValue

template <typename T>
T ConfigManager::
getValue(const string &group, const string &key) {
    for (auto const &it : ptrees) {
        const ptree &pt = it.second;
        if (pt.get_child_optional(group+"."+key)) {
            return pt.get<T>(group+"."+key);
        }
    }
    REPORT_ERROR("There is no key \"" << key << "\" in group \"" << group << "\"!");
} // getValue

template <typename T>
T ConfigManager::
getValue(const string &group, const string &key, const T &defaultValue) {
    if (!hasGroup(group) || !hasKey(group, key)) {
        return defaultValue;
    }
    for (auto const &it : ptrees) {
        const ptree &pt = it.second;
        if (pt.get_child_optional(group+"."+key)) {
            return pt.get<T>(group+"."+key);
        }
    }
} // getValue

}

#endif // __GEOMTK_ConfigManager__
