#ifndef __RLLScalarField__
#define __RLLScalarField__

#include "StructuredScalarField.h"

namespace geomtk {

/**
 * RLLScalarField
 * This class describes the scalar field on the RLL mesh.
 */

class RLLScalarField : public StructuredScalarField {
public:
    RLLScalarField(const Mesh &mesh, bool hasHalfLevel = false);
    virtual ~RLLScalarField();
};

}

#endif

