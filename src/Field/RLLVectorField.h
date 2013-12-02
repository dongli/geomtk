#ifndef __RLLVectorField__
#define __RLLVectorField__

#include "StructuredVectorField.h"
#include "RLLMesh.h"

class RLLVectorField : public StructuredVectorField {
public:
    RLLVectorField(Mesh &mesh);
    virtual ~RLLVectorField();
};

#endif
