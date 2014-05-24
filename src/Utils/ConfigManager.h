#ifndef __Geomtk_ConfigManager__
#define __Geomtk_ConfigManager__

#include "geomtk_commons.h"

namespace geomtk {

class ConfigPack {
public:
    string filePath;
    string name;
    map<string, variant<string, double, bool> > keyValues;
};

class ConfigManager {
    list<ConfigPack> configPacks;
public:
    ConfigManager();
    virtual ~ConfigManager();

    void parse(const string &filePath);

    /**
     *  Check if a key is in a given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *
     *  @return Boolean status.
     */
    bool hasKey(const string &packName, const string &key) const;

    /**
     *  Get the value of a given key in the given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *  @param value    the value.
     */
    void getValue(const string &packName, const string &key,
                  string &value) const;

    /**
     *  Get the value of a given key in the given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *  @param value    the value.
     */
    void getValue(const string &packName, const string &key,
                  double &value) const;

    /**
     *  Get the value of a given key in the given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *  @param value    the value.
     */
    void getValue(const string &packName, const string &key,
                  int &value) const;

    /**
     *  Get the value of a given key in the given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *  @param value    the value.
     */
    void getValue(const string &packName, const string &key,
                  bool &value) const;

    void print() const;
private:
    const ConfigPack& getPack(const string &packName) const;
};

}

#endif // __Geomtk_ConfigManager__