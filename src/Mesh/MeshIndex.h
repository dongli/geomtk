#ifndef __GEOMTK_MeshIndex__
#define __GEOMTK_MeshIndex__

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
    uword numDim;
    const MeshType *mesh;
public:
    MeshIndex();
    MeshIndex(uword numDim);
    virtual ~MeshIndex();

    virtual void
    init(uword numDim);

    virtual MeshIndex&
    operator=(const MeshIndex &other);

    /**
     *  Locate the given space coordinate in the mesh.
     */
    virtual void
    locate(const MeshType &mesh, const CoordType &x) = 0;

    virtual uword
    cellIndex(const MeshType &mesh, int loc) const = 0;

    virtual bool
    isValid() const = 0;

    virtual void
    print() const = 0;
}; // MeshIndex

} // geomtk

#include "MeshIndex-impl.h"

#endif // __GEOMTK_MeshIndex__
