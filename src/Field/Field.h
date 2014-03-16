#ifndef __Field__
#define __Field__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

namespace geomtk {

#define UPDATE_HALF_LEVEL true

/**
 *  This is the base class for specifying field on a mesh.
 */

class Field {
protected:
    string name;
    string longName;
    string units;
    const Mesh *mesh;
    bool hasHalfLevel;
public:
    Field();
    virtual ~Field();

    void common(const string &name, const string &units, const string &longName,
                const Mesh &mesh, bool hasHalfLevel = false);

    void setName(const string &name);
    void setLongName(const string &longName);
    void setUnits(const string &units);
    const string& getName() const;
    const string& getLongName() const;
    const string& getUnits() const;
    const Mesh& getMesh() const;

    virtual int getStaggerLocation() const = 0;
};

}

#endif
