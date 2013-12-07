#include "Mesh.h"

namespace geomtk {

Mesh::Mesh(Domain &domain) {
    this->domain = &domain;
}

Mesh::~Mesh() {
}

const Domain& Mesh::getDomain() const {
    return *domain;
}

}
