#include "RLLVectorField.h"

namespace geomtk {

RLLVectorField::RLLVectorField(Mesh &mesh) : StructuredVectorField(mesh) {
    if (dynamic_cast<RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be RLLMesh!")
    }
}

RLLVectorField::~RLLVectorField() {
}

}
