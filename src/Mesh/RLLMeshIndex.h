#ifndef __RLLMeshIndex__
#define __RLLMeshIndex__

#include "StructuredMeshIndex.h"
#include "RLLMesh.h"
#include "SphereDomain.h"

namespace geomtk {

class RLLMeshIndex : public StructuredMeshIndex {
protected:
    Pole pole;
    bool inPolarCap;
    bool onPole;
public:
    RLLMeshIndex(int numDim);
    virtual ~RLLMeshIndex();

    virtual void reset();

    Pole getPole() const;
    bool isInPolarCap() const;
    bool isOnPole() const;

    /**
     * @brief Inherit StructuredMeshIndex::locate(SpaceCoord) and add Pole
     * judgement.
     * @see MeshIndex::locate(SpaceCoord)
     */
    virtual void locate(const Mesh &mesh, const SpaceCoord &x);

    virtual void print() const;
};

}

#endif
