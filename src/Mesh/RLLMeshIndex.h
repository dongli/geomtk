#ifndef __GEOMTK_RLLMeshIndex__
#define __GEOMTK_RLLMeshIndex__

#include "StructuredMeshIndex.h"
#include "RLLMesh.h"
#include "SphereCoord.h"

namespace geomtk {

class RLLMeshIndex : public StructuredMeshIndex<RLLMesh, SphereCoord> {
public:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
    Pole _pole;
    bool inPolarCap;
    bool onPole;
    bool moveOnPole;
public:
    RLLMeshIndex();
    RLLMeshIndex(uword numDim);
    virtual ~RLLMeshIndex();

    virtual void
    reset();

    virtual RLLMeshIndex&
    operator=(const RLLMeshIndex &other);

    /**
     *  Toggle 'moveOnPole' boolean.
     */
    void
    setMoveOnPole(bool moveOnPole) {
        this->moveOnPole = moveOnPole;
    }

    Pole
    pole() const {
        return _pole;
    }

    bool
    isInPolarCap() const {
        return inPolarCap;
    }

    bool
    isOnPole() const {
        return onPole;
    }

    bool
    isMoveOnPole() const {
        return moveOnPole;
    }

    /**
     *  Inherit CartesianMeshIndex::locate(SpaceCoord) and add Pole judgement.
     *
     *  @param mesh the mesh that should be RLLMesh.
     *  @param x    the coordinate that should be a spherical coordinate.
     *
     *  @return None.
     */
    virtual void
    locate(const RLLMesh &mesh, const SphereCoord &x);

    virtual void
    print() const;
}; // RLLMeshIndex

} // geomtk

#endif // __GEOMTK_RLLMeshIndex__
