#ifndef __GEOMTK_ConfigManager__
#define __GEOMTK_ConfigManager__

#include "geomtk_commons.h"

namespace geomtk {

class ConfigManager {
    string filePath;
    boost::property_tree::ptree pt;
public:
    ConfigManager();
    virtual ~ConfigManager();

    /**
     *  Parse the configuration from a given file.
     *
     *  @param filePath the configure file path.
     *  @param mute     the control boolean.
     */
    void parse(const string &filePath, bool mute = false);

    template <typename T>
    void addKeyValue(const string &group, const string &key, const T &value);

    bool hasGroup(const string &group) const;

    vector<string> getGroups() const;

    bool hasKey(const string &group, const string &key) const;

    vector<string> getKeys(const string &group) const;

    template <typename T>
    T getValue(const string &group, const string &key) const;

    template <typename T>
    T getValue(const string &group, const string &key, const T &defaultValue) const;

    void print() const;
private:
    void printPropertyTree(const boost::property_tree::ptree &pt,
                           int level = 0) const;
};

template <typename T>
void ConfigManager::addKeyValue(const string &group, const string &key,
                                const T &value) {
    if (hasKey(group, key)) {
        REPORT_ERROR("Already add key \"" << key << "\" in group \"" <<
                     group << "\"!");
    }
    pt.put(group+"."+key, value);
}

template <typename T>
T ConfigManager::getValue(const string &group, const string &key) const {
    if (!hasGroup(group)) {
        printPropertyTree(pt);
        REPORT_ERROR("Configure file \"" << filePath <<
                     "\" does not have group \"" << group << "\"!");
    }
    if (!hasKey(group, key)) {
        REPORT_ERROR("Configure file \"" << filePath <<
                     "\" does not have key \"" << key << "\" in group \"" <<
                     group << "\"!");
    }
    return pt.get<T>(group+"."+key);
}

template <typename T>
T ConfigManager::
getValue(const string &group, const string &key, const T &defaultValue) const {
    if (!hasGroup(group) || !hasKey(group, key)) {
        return defaultValue;
    }
    return pt.get<T>(group+"."+key);
}

}

#endif // __GEOMTK_ConfigManager__
