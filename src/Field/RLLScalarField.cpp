#include "RLLScalarField.h"
#include "RLLMesh.h"

RLLScalarField::RLLScalarField(Mesh &mesh) : StructuredScalarField(mesh) {
    if (dynamic_cast<RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply RLLMesh!");
    }
}

RLLScalarField::~RLLScalarField() {
}
