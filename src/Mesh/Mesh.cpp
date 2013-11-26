#include "Mesh.h"

Mesh::Mesh(Domain &domain) {
    this->domain = &domain;
}

Mesh::~Mesh() {
}

const Domain& Mesh::getDomain() {
    return *domain;
}
