#ifndef __RLLMeshIndex__
#define __RLLMeshIndex__

#include "StructuredMeshIndex.h"
#include "RLLMesh.h"

class RLLMeshIndex : public StructuredMeshIndex {
protected:
public:
    RLLMeshIndex(Mesh &mesh);
    virtual ~RLLMeshIndex();

    virtual void locate(SpaceCoord &x);
};

#endif
