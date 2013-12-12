#ifndef __Field__
#define __Field__

#include "geomtk_commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

namespace geomtk {

#define UPDATE_HALF_LEVEL true

/**
 * Field
 * This is the base class for describing field on a mesh.
 */

class Field {
protected:
    const Mesh *mesh;
public:
    Field(const Mesh &mesh, bool hasHalfLevel = false);
    virtual ~Field();

    /**
     * Apply the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel, bool updateHalfLevel = false) = 0;

    /**
     * Return the underlying mesh.
     * @return The mesh object.
     */
    const Mesh& getMesh() const;
};

}

#endif
