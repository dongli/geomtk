#ifndef __GEOMTK_Regrid__
#define __GEOMTK_Regrid__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "Field.h"
#include "MeshIndex.h"

namespace geomtk {

enum RegridMethod {
    LINEAR, QUADRATIC, CUBIC
};

/**
 * @class Regrid
 * @brief This is the base class that provides regridding operations. Concrete
 * regrid child classes will be bounded to a specific mesh. 
 */
template <class MeshType, class CoordType>
class Regrid {
protected:
    static const MeshType *_mesh;
public:
    Regrid() {}
    Regrid(const MeshType &mesh) {
        _mesh = &mesh;
    }
    virtual ~Regrid() {}

    const MeshType&
    mesh() const {
        return *_mesh;
    }

    static void
    init(const MeshType &mesh) {
        _mesh = &mesh;
    }

    static RegridMethod
    methodFromString(const string &method) {
        if (method == "linear") {
            return LINEAR;
        } else if (method == "quadratic") {
            return QUADRATIC;
        } else if (method == "cubic") {
            return CUBIC;
        } else {
            REPORT_ERROR("Unknown regrid method \"" << method << "\"!");
        }
    }
}; // Regrid

template <class MeshType, class CoordType>
const MeshType* Regrid<MeshType, CoordType>::_mesh = NULL;

} // geomtk

#endif // __GEOMTK_Regrid__
