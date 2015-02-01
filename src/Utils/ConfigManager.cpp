#include "ConfigManager.h"

namespace geomtk {

ConfigManager::ConfigManager() {
    filePath = "N/A";
    REPORT_ONLINE;
}

ConfigManager::~ConfigManager() {
    REPORT_OFFLINE;
}

void ConfigManager::parse(const string &filePath, bool mute) {
    this->filePath = filePath;
    try {
        if (filePath.rfind(".json") != string::npos) {
            read_json(filePath, pt);
        } else if (filePath.rfind(".xml") != string::npos) {
            read_xml(filePath, pt);
        } else {
            read_json(filePath, pt);
        }
    } catch (std::exception &e) {
        REPORT_ERROR("Failed to parse \"" << filePath << "\" due to:\n" <<
                     e.what());
    }
    if (!mute) {
        REPORT_NOTICE("Parse \"" << filePath << "\".");
    }
}

bool ConfigManager::hasGroup(const string &group) const {
    if (pt.get_child_optional(group)) {
        return true;
    } else {
        return false;
    }
}

vector<string> ConfigManager::getGroups() const {
    vector<string> groups;
    for (auto i = pt.begin(); i != pt.end(); ++i) {
        groups.push_back(i->first);
    }
    return groups;
}

bool ConfigManager::hasKey(const string &group, const string &key) const {
    if (pt.get_child_optional(group+"."+key)) {
        return true;
    } else {
        return false;
    }
}

vector<string> ConfigManager::getKeys(const string &group) const {
    vector<string> keys;
    const boost::property_tree::ptree &gt = pt.get_child(group);
    for (auto i = gt.begin(); i != gt.end(); ++i) {
        keys.push_back(i->first);
    }
    return keys;
}

void ConfigManager::print() const {
    if (filePath != "N/A") {
        cout << "Configure list in \"" << filePath << "\":" << endl;
    }
    printPropertyTree(pt);
}

void ConfigManager::printPropertyTree(const boost::property_tree::ptree &pt,
                                      int level) const {
    assert(level < 2);
    string s(level*2, ' ');
    level++;
    for (auto i = pt.begin(); i != pt.end(); ++i) {
        cout << s;
        if (level == 1) {
            cout << i->first << ":" << endl;
            printPropertyTree(i->second, level);
        } else {
            cout << i->first << ": ";
            cout << i->second.get_value<string>() << endl;
        }
    }
    level--;
}

}
