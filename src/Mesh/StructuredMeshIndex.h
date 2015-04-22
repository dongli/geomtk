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
    StructuredMeshIndex(int numDim);
    virtual ~StructuredMeshIndex();

    virtual void
    setNumDim(int numDim);

    /**
     *  Reset the indices to undefined status.
     */
    virtual void
    reset();

    virtual int
    operator()(int axisIdx, int gridType) const;

    virtual int&
    operator()(int axisIdx, int gridType);

    virtual StructuredMeshIndex&
    operator=(const StructuredMeshIndex &other);

    virtual void
    locate(const MeshType &mesh, const CoordType &x);

    virtual int
    getIndex(const MeshType &mesh, int loc) const;

    virtual bool
    isValid() const;

    virtual void
    print() const;
}; // StructuredMeshIndex

} // geomtk

#include "StructuredMeshIndex-impl.h"

#endif // __GEOMTK_StructuredMeshIndex__
