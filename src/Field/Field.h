#ifndef __Field__
#define __Field__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

namespace geomtk {

enum FieldType {
    ScalarField, VectorField
};

#define _1D 1
#define _2D 2
#define _3D 3

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
    Field(const Mesh &mesh, bool hasHalfLevel = false);
    Field(const string &name, const string &units, const string &longName,
          const Mesh &mesh, bool hasHalfLevel = false);
    virtual ~Field();

    void setName(const string &name);
    void setLongName(const string &longName);
    void setUnits(const string &units);
    const string& getName() const;
    const string& getLongName() const;
    const string& getUnits() const;
    const Mesh& getMesh() const;
};

}

#endif
