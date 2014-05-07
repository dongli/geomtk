#include "ConfigManager.h"

namespace geomtk {

ConfigManager::ConfigManager() {
    REPORT_ONLINE;
}

ConfigManager::~ConfigManager() {
    REPORT_OFFLINE;
}

/**
 *  The format of configuration file should be:
 *
 *      <key> = <value> \
 *              <continued-value>
 */
void ConfigManager::parse(const string &filePath) {
    // some patterns
    regex reWhiteSpace("\\s");
    regex reContinueChar("\\\\\\s*$");
    regex reEmptyLine("^(\\s|\\\\)*$");
    regex rePackStart("^(\\w+): *$");
    regex reKeyPart("^\\s*(\\w)\\s*=");
    regex reValuePart("=\\s*(.*)$");
    regex reNumerics("(\\+|-)?\\d+(\\.\\d+(e(\\+|-)\\d+)?)?");
    regex reString("(\"(?:[^\"\\\\]|\\\\.)*\"\\s*\\\\?\\s*)+");
    regex reStringJunk("(^\"|\"$|\"\\s*\\\\?\\s*\")");
    match_results<std::string::const_iterator> what;

    ifstream file;
    file.open(filePath, std::ios::in);
    if (!file.good()) {
        REPORT_ERROR("Fail to open \"" << filePath << "\"!");
    }

    string line;
    while (!file.eof()) {
        getline(file, line);
        // check if the last non-empty character is '\' for continuing line
        while (regex_search(line, reContinueChar)) {
            string continueLine;
            getline(file, continueLine);
            if (regex_search(continueLine, reEmptyLine)) {
                continue;
            }
            line += continueLine;
        }
        // omit empty line
        if (regex_search(line, reEmptyLine)) {
            continue;
        }
        // check if new configuration pack starts
        if (regex_search(line, what, rePackStart)) {
            ConfigPack pack;
            pack.filePath = filePath;
            pack.name = what[1];
            configPacks.push_back(pack);
            continue;
        }
        ConfigPack &pack = configPacks.back();
        // organize the key value pair
        string key, value;
        if (regex_search(line, what, reKeyPart)) {
            key = what[1];
        }
        if (pack.keyValues.count(key) != 0) {
            REPORT_ERROR("There are duplicate key \"" << key <<
                         "\" in \"" << filePath << "\"!");
        }
        if (regex_search(line, what, reValuePart)) {
            value = what[1];
            if (regex_match(value, reNumerics)) {
                pack.keyValues[key] = atof(value.c_str());
            } else if (regex_match(value, reString)) {
                pack.keyValues[key] = regex_replace(value, reStringJunk, "");
            }
        }
    }

    file.close();
}

void ConfigManager::getValue(const string &packName, const string &key,
                             string &value) {
    ConfigPack &pack = getPack(packName);
    if (pack.keyValues.count(key) == 0) {
        REPORT_ERROR("No key \"" << key << "\" of pack \"" << pack.name <<
                     "\" in \"" << pack.filePath << "\"!");
    }
    if (pack.keyValues[key].which() != 0) {
        REPORT_ERROR("Key \"" << key << "\" of pack \"" << pack.name <<
                     "\" in \"" << pack.filePath << "\" is not a string!");
    }
    value = boost::get<string>(pack.keyValues[key]);
}

void ConfigManager::getValue(const string &packName, const string &key,
                             double &value) {
    ConfigPack &pack = getPack(packName);
    if (pack.keyValues.count(key) == 0) {
        REPORT_ERROR("No key \"" << key << "\" in \"" << pack.filePath << "\"!");
    }
    if (pack.keyValues[key].which() != 1) {
        REPORT_ERROR("Key \"" << key << "\" of pack \"" << pack.name <<
                     "\" in \"" << pack.filePath << "\" is not a double!");
    }
    value = boost::get<double>(pack.keyValues[key]);
}

void ConfigManager::print() const {
    list<ConfigPack>::const_iterator pack;
    for (pack = configPacks.begin(); pack != configPacks.end(); ++pack) {
        cout << "ConfigPack \"" << pack->name << "\":" << endl;
        map<string, variant<string, double> >::const_iterator keyValue;
        for (keyValue = pack->keyValues.begin();
             keyValue != (*pack).keyValues.end(); ++keyValue) {
            cout << "  " << keyValue->first << " = " << keyValue->second << endl;
        }
    }
}

ConfigPack& ConfigManager::getPack(const string &packName) {
    list<ConfigPack>::iterator pack;
    for (pack = configPacks.begin(); pack != configPacks.end(); ++pack) {
        if (pack->name == packName) {
            return *pack;
        }
    }
    REPORT_ERROR("Unknown configuration pack \"" << packName << "\"!");
}

}