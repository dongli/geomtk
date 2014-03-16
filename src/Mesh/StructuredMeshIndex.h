#ifndef __Geomtk_StructuredMeshIndex__
#define __Geomtk_StructuredMeshIndex__

#include "MeshIndex.h"
#include "StructuredMesh.h"

namespace geomtk {

class StructuredMeshIndex : public MeshIndex {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    int **indices;
    int numDim;
public:
    StructuredMeshIndex(int numDim);
    virtual ~StructuredMeshIndex();

    /**
     *  Reset the indices to undefined status.
     */
    virtual void reset();

    virtual int operator()(int axisIdx, int gridType) const;
    virtual int& operator()(int axisIdx, int gridType);

    virtual StructuredMeshIndex& operator=(const StructuredMeshIndex &other);

    /**
     *  MeshIndex::locate(SpaceCoord x)
     */
    virtual void locate(const Mesh &mesh, const SpaceCoord &x);

    virtual void print() const;
};

}

#endif
