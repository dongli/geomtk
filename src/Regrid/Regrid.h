#ifndef __Geomtk_Regrid__
#define __Geomtk_Regrid__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "Field.h"
#include "MeshIndex.h"

namespace geomtk {

enum RegridMethod {
    BILINEAR, TRILINEAR
};

/**
 * @class Regrid
 * @brief This is the base class that provides regridding operations. Concrete
 * regrid child classes will be bounded to a specific mesh. 
 */
template <class MeshType, class CoordType>
class Regrid {
protected:
    const MeshType *mesh;
public:
    Regrid(const MeshType &mesh) { this->mesh = &mesh; }
    virtual ~Regrid() {}

    const MeshType& getMesh() const { return *mesh; }
};

}

#endif
