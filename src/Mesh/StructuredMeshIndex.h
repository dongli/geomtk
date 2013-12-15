#ifndef __StructuredMeshIndex__
#define __StructuredMeshIndex__

#include "MeshIndex.h"
#include "StructuredMesh.h"

namespace geomtk {

class StructuredMeshIndex : public MeshIndex {
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

    virtual int operator()(int dim, StaggerType staggerType) const;
    virtual int& operator()(int dim, StaggerType staggerType);

    virtual StructuredMeshIndex& operator=(const StructuredMeshIndex &other);

    /**
     *  MeshIndex::locate(SpaceCoord x)
     */
    virtual void locate(const Mesh &mesh, const SpaceCoord &x);

    virtual void print() const;
};

}

#endif
