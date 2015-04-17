#ifndef __Geomtk_Regrid__
#define __Geomtk_Regrid__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "Field.h"
#include "MeshIndex.h"

namespace geomtk {

enum RegridMethod {
    BILINEAR, BIQUADRATIC, BICUBIC
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
        if (method == "bilinear") {
            return BILINEAR;
        } else if (method == "biquadratic") {
            return BIQUADRATIC;
        } else if (method == "bicubic") {
            return BICUBIC;
        } else {
            REPORT_ERROR("Unknown regrid method \"" << method << "\"!");
        }
    }
};

template <class MeshType, class CoordType>
const MeshType* Regrid<MeshType, CoordType>::_mesh;

}

#endif
