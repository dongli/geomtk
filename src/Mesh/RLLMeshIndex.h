#ifndef __RLLMeshIndex__
#define __RLLMeshIndex__

#include "StructuredMeshIndex.h"
#include "RLLMesh.h"
#include "SphereDomain.h"

class RLLMeshIndex : public StructuredMeshIndex {
protected:
    Pole pole;
    bool inPolarCap;
    bool onPole;
public:
    RLLMeshIndex(Mesh &mesh);
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
    virtual void locate(const SpaceCoord &x);

    virtual void print();
};

#endif
