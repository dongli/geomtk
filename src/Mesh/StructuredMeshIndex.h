#ifndef __StructuredMeshIndex__
#define __StructuredMeshIndex__

#include "MeshIndex.h"
#include "StructuredMesh.h"

namespace geomtk {

class StructuredMeshIndex : public MeshIndex {
protected:
    int **indices;
public:
    StructuredMeshIndex(Mesh &mesh);
    virtual ~StructuredMeshIndex();

    /**
     * @brief Reset the indices to undefined status.
     */
    virtual void reset();

    virtual int operator()(int dim, StaggerType staggerType) const;
    virtual int& operator()(int dim, StaggerType staggerType);

    /**
     * @see MeshIndex::locate(SpaceCoord x)
     */
    virtual void locate(const SpaceCoord &x);

    virtual void print() const;
};

}

#endif
