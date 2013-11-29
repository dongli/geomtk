#ifndef __MeshIndex__
#define __MeshIndex__

#include "commons.h"
#include "Mesh.h"

#define UNDEFINED_MESH_INDEX -99999

/**
 * @class MeshIndex
 * @brief This class describes the index of grid cell that a point is at.
 */

class MeshIndex {
protected:
    Mesh *mesh;
public:
    MeshIndex(Mesh &mesh);
    virtual ~MeshIndex();

    /**
     * @brief Locate the given space coordinate in the mesh.
     */
    virtual void locate(SpaceCoord &x) = 0;
};

#endif
