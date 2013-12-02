#ifndef __Field__
#define __Field__

#include "commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

/**
 * Field
 * This is the base class for describing field on a mesh.
 */

class Field {
protected:
    Mesh *mesh;
public:
    Field(Mesh &mesh);
    virtual ~Field();

    /**
     * Apply the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel) = 0;

    /**
     * Return the underlying mesh.
     * @return The mesh object.
     */
    const Mesh& getMesh();
};

#endif
