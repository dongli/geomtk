#include "ConfigManager.h"

namespace geomtk {

map<string, ptree> ConfigManager::ptrees;

void ConfigManager::
parse(const string &filePath, bool mute) {
    if (ptrees.find(filePath) != ptrees.end()) {
        REPORT_ERROR("File \"" << filePath << "\" has already been parsed!");
    } else {
        ptree pt;
        ptrees[filePath] = pt;
    }
    try {
        if (filePath.rfind(".json") != string::npos) {
            read_json(filePath, ptrees[filePath]);
        } else if (filePath.rfind(".xml") != string::npos) {
            read_xml(filePath, ptrees[filePath]);
        } else {
            read_json(filePath, ptrees[filePath]);
        }
    } catch (std::exception &e) {
        REPORT_ERROR("Failed to parse \"" << filePath << "\" due to:\n" <<
                     e.what());
    }
    if (!mute) {
        REPORT_NOTICE("Parse \"" << filePath << "\".");
    }
} // parse

bool ConfigManager::
hasGroup(const string &group) {
    for (auto const &it : ptrees) {
        if (it.second.get_child_optional(group)) {
            return true;
        }
    }
    return false;
} // hasGroup

vector<string> ConfigManager::
getGroups() {
    vector<string> groups;
    for (auto const &it : ptrees) {
        for (auto i = it.second.begin(); i != it.second.end(); ++i) {
            groups.push_back(i->first);
        }
    }
    return groups;
} //getGroups

bool ConfigManager::
hasKey(const string &group, const string &key) {
    for (auto const &it : ptrees) {
        if (it.second.get_child_optional(group+"."+key)) {
            return true;
        }
    }
    return false;
} // hasKey

vector<string> ConfigManager::
getKeys(const string &group) {
    vector<string> keys;
    for (auto const &it : ptrees) {
        if (it.second.get_child_optional(group)) {
            const ptree &gt = it.second.get_child(group);
            for (auto i = gt.begin(); i != gt.end(); ++i) {
                keys.push_back(i->first);
            }
            break;
        }
    }
    return keys;
} // getKeys

void ConfigManager::
print() {
    for (auto const &it : ptrees) {
        cout << it.first << ":" << endl;
        printPropertyTree(it.second);
    }
} // print

void ConfigManager::
printPropertyTree(const boost::property_tree::ptree &pt, int level) {
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
} // printPropertyTree

} // geomtk
