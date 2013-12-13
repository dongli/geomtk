#include "RLLVectorField.h"

namespace geomtk {

RLLVectorField::RLLVectorField(const Mesh &mesh, bool hasHalfLevel)
        : StructuredVectorField(mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!")
    }
}

RLLVectorField::RLLVectorField(const string &name, const string &units,
                               const string &longName, const Mesh &mesh,
                               bool hasHalfLevel)
    : StructuredVectorField(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!")
    }
}

RLLVectorField::~RLLVectorField() {
}

}
