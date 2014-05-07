#ifndef __Geomtk_ConfigManager__
#define __Geomtk_ConfigManager__

#include "geomtk_commons.h"

namespace geomtk {

class ConfigPack {
public:
    string filePath;
    string name;
    map<string, variant<string, double> > keyValues;
};

class ConfigManager {
    list<ConfigPack> configPacks;
public:
    ConfigManager();
    virtual ~ConfigManager();

    void parse(const string &filePath);

    void getValue(const string &packName, const string &key, string &value);
    void getValue(const string &packName, const string &key, double &value);

    void print() const;
private:
    ConfigPack& getPack(const string &packName);
};

}

#endif // __Geomtk_ConfigManager__