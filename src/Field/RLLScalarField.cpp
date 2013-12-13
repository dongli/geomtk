#include "RLLScalarField.h"
#include "RLLMesh.h"

namespace geomtk {

RLLScalarField::RLLScalarField(const Mesh &mesh, bool hasHalfLevel)
        : StructuredScalarField(mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply RLLMesh!");
    }
}

RLLScalarField::RLLScalarField(const string &name, const string &units,
                               const string &longName, const Mesh &mesh,
                               bool hasHalfLevel)
    : StructuredScalarField(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply RLLMesh!");
    }
}
    
RLLScalarField::~RLLScalarField() {
}

}
