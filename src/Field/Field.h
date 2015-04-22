#ifndef __GEOMTK_Field__
#define __GEOMTK_Field__

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
    string _name;
    string _longName;
    string _units;
    int _numDim;
    const MeshType *_mesh;
    bool hasHalfLevel;
    bool createTag;
public:
    Field();
    virtual ~Field() {}

    void
    create(const string &name, const string &units, const string &longName,
           const MeshType &mesh, int numDim, bool hasHalfLevel = false);

    bool
    isCreated() const {
        return createTag;
    }

    void
    setName(const string &name) {
        _name = name;
    }

    void setLongName(const string &longName) {
        _longName = longName;
    }

    void
    setUnits(const string &units) {
        _units = units;
    }

    const string&
    name() const {
        return _name;
    }

    const string&
    longName() const {
        return _longName;
    }

    const string&
    units() const {
        return _units;
    }

    int
    numDim() const {
        return _numDim;
    }

    const MeshType&
    mesh() const {
        return *_mesh;
    }

    Field&
    operator=(const Field &other);

    virtual int
    staggerLocation() const = 0;
}; // Field

} // geomtk

#include "Field-impl.h"

#endif // __GEOMTK_Field__
