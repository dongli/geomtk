#ifndef __GEOMTK_StructuredMeshIndex__
#define __GEOMTK_StructuredMeshIndex__

#include "MeshIndex.h"
#include "StructuredMesh.h"

namespace geomtk {

template <class MeshType, class CoordType>
class StructuredMeshIndex : public MeshIndex<MeshType, CoordType> {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    int **indices;
public:
    StructuredMeshIndex();
    StructuredMeshIndex(uword numDim);
    virtual ~StructuredMeshIndex();

    virtual void
    init(uword numDim);

    /**
     *  Reset the indices to undefined status.
     */
    virtual void
    reset();

    virtual int
    operator()(uword axisIdx, int gridType) const;

    virtual int&
    operator()(uword axisIdx, int gridType);

    virtual StructuredMeshIndex&
    operator=(const StructuredMeshIndex &other);

    virtual void
    locate(const MeshType &mesh, const CoordType &x);

    virtual uword
    cellIndex(const MeshType &mesh, int loc) const;

    virtual bool
    isValid() const;

    virtual bool
    atBoundary(const MeshType &mesh) const;

    virtual void
    print() const;
}; // StructuredMeshIndex

} // geomtk

#include "StructuredMeshIndex-impl.h"

#endif // __GEOMTK_StructuredMeshIndex__
