#ifndef __RLLMesh__
#define __RLLMesh__

#include "StructuredMesh.h"

class RLLMesh : public StructuredMesh {
protected:
    double poleRadius; // virtual effective radius of pole
public:
    RLLMesh(Domain &domain);
    virtual ~RLLMesh();

    void setPoleRadius(double radius);
};

#endif
