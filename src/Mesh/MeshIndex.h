#ifndef __MeshIndex__
#define __MeshIndex__

#include "geomtk_commons.h"
#include "Mesh.h"

namespace geomtk {

#define UNDEFINED_MESH_INDEX -999

/**
 * @class MeshIndex
 * @brief This class describes the index of grid cell that a point is at.
 */

class MeshIndex {
public:
    MeshIndex(int numDim);
    virtual ~MeshIndex();

    virtual MeshIndex& operator=(const MeshIndex &other);

    /**
     * @brief Locate the given space coordinate in the mesh.
     */
    virtual void locate(const Mesh &mesh, const SpaceCoord &x) = 0;

    virtual void print() const = 0;
};

}

#endif
