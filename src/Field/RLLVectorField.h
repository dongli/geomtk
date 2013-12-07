#ifndef __RLLVectorField__
#define __RLLVectorField__

#include "StructuredVectorField.h"
#include "RLLMesh.h"

namespace geomtk {

/**
 * RLLVectorField
 * This class describes the vector field on the RLL mesh.
 */

class RLLVectorField : public StructuredVectorField {
public:
    RLLVectorField(Mesh &mesh);
    virtual ~RLLVectorField();
};

}

#endif
