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

class Regrid {
protected:
    const Mesh *mesh;
public:
    Regrid(const Mesh &mesh) { this->mesh = &mesh; }
    virtual ~Regrid() {}
};

}

#endif
