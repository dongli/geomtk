#include "RLLVectorField.h"

namespace geomtk {

RLLVectorField::RLLVectorField(const Mesh &mesh, bool hasHalfLevel)
        : StructuredVectorField(mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!")
    }
}

RLLVectorField::~RLLVectorField() {
}

}
