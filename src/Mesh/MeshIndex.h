#ifndef __Geomtk_MeshIndex__
#define __Geomtk_MeshIndex__

#include "geomtk_commons.h"
#include "Mesh.h"

namespace geomtk {

#define UNDEFINED_MESH_INDEX -999

/**
 *  This class describes the index of grid cell that a point is at.
 */
template <class MeshType, class CoordType>
class MeshIndex {
protected:
    int numDim;
    const MeshType *mesh;
public:
    MeshIndex(int numDim);
    virtual ~MeshIndex();

    virtual MeshIndex& operator=(const MeshIndex &other);

    /**
     *  Locate the given space coordinate in the mesh.
     */
    virtual void locate(const MeshType &mesh, const CoordType &x) = 0;

    virtual int getIndex(const MeshType &mesh, int loc) const = 0;

    virtual void print() const = 0;
};

} // geomtk

#include "MeshIndex-impl.h"

#endif // __Geomtk_MeshIndex__
