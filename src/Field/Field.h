#ifndef __Field__
#define __Field__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

namespace geomtk {

enum FieldType {
    ScalarField, VectorField
};

#define UPDATE_HALF_LEVEL true

/**
 *  This is the base class for describing field on a mesh.
 */

class Field {
protected:
    string name;
    string units;
    string longName;
    const Mesh *mesh;
    bool hasHalfLevel;
public:
    Field(const Mesh &mesh, bool hasHalfLevel = false);
    Field(const string &name, const string &units, const string &longName,
          const Mesh &mesh, bool hasHalfLevel = false);
    virtual ~Field();

    const Mesh& getMesh() const;
};

}

#endif
