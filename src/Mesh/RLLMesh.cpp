#include "RLLMesh.h"
#include "SphereDomain.h"

RLLMesh::RLLMesh(Domain &domain) : StructuredMesh(domain) {
    if (dynamic_cast<SphereDomain*>(&domain) == NULL) {
        REPORT_ERROR("The domain should be SphereDomain!")
    }
}

RLLMesh::~RLLMesh() {
}

void RLLMesh::setPoleRadius(double radius) {
    poleRadius = radius;
}
