#ifndef __Geomtk_ConfigManager__
#define __Geomtk_ConfigManager__

#include "geomtk_commons.h"

namespace geomtk {

class ConfigPack {
public:
    string filePath;
    string name;
    map<string, any> keyValues;
};

class ConfigManager {
    list<ConfigPack> configPacks;
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
    void addKeyValue(const string &packName, const string &key,
                     const T &value);

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
     *  Return all the keys of a given pack.
     *
     *  @param packName the pack name.
     *
     *  @return The vector of the keys.
     */
    vector<string> getKeys(const string &packName) const;

    /**
     *  Get the value of a given key in the given pack.
     *
     *  @param packName the pack name.
     *  @param key      the key.
     *  @param value    the value.
     */
    template <typename T>
    void getValue(const string &packName, const string &key,
                  T &value) const;

    void print() const;
private:
    const ConfigPack& getPack(const string &packName) const;

    bool hasPack(const string &packName) const;
};

template <typename T>
void ConfigManager::addKeyValue(const string &packName, const string &key,
                                const T &value) {
    if (!hasPack(packName)) {
        ConfigPack pack;
        pack.name = packName;
        configPacks.push_back(pack);
    }
    any value_ = value;
    configPacks.back().keyValues[key] = value_;
}

template <typename T>
void ConfigManager::getValue(const string &packName, const string &key,
                             T &value) const {
    const ConfigPack &pack = getPack(packName);
    if (pack.keyValues.count(key) == 0) {
        REPORT_ERROR("No key \"" << key << "\" of pack \"" << pack.name <<
                     "\" in \"" << pack.filePath << "\"!");
    }
    if (pack.keyValues.at(key).type() != typeid(T)) {
        REPORT_ERROR("Value for key \"" << key << "\" of pack \"" <<
                     pack.name << "\" in \"" << pack.filePath <<
                     "\" does not match type " << typeid(T).name() << "!");
    }
    value = boost::any_cast<T>(pack.keyValues.at(key));
}

}

#endif // __Geomtk_ConfigManager__
