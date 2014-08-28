#ifndef __Geomtk_Field__
#define __Geomtk_Field__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

namespace geomtk {

#define UPDATE_HALF_LEVEL true

/**
 *  This is the base class for specifying field on a mesh.
 */
template <class MeshType_>
class Field {
public:
    typedef MeshType_ MeshType;
protected:
    string name;
    string longName;
    string units;
    int numDim;
    const MeshType *mesh;
    bool hasHalfLevel;
    bool createTag;
public:
    Field();
    virtual ~Field() {}

    void create(const string &name, const string &units, const string &longName,
                const MeshType &mesh, int numDim, bool hasHalfLevel = false);

    bool isCreated() const { return createTag; }

    void setName(const string &name) { this->name = name; }

    void setLongName(const string &longName) { this->longName = longName; }

    void setUnits(const string &units) { this->units = units; }

    const string& getName() const { return name; }

    const string& getLongName() const { return longName; }

    const string& getUnits() const { return units; }

    int getNumDim() const { return numDim; }

    const MeshType& getMesh() const { return *mesh; }

    Field& operator=(const Field &other);

    virtual int getStaggerLocation() const = 0;
};

} // geomtk

#include "Field-impl.h"

#endif // __Geomtk_Field__
