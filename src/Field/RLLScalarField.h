#ifndef __RLLScalarField__
#define __RLLScalarField__

#include "StructuredScalarField.h"

class RLLScalarField : public StructuredScalarField {
public:
    RLLScalarField(Mesh &mesh);
    virtual ~RLLScalarField();
};

#endif

