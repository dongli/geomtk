#include "Mesh.h"

namespace geomtk {

Mesh::Mesh(Domain &domain) {
    this->domain = &domain;
    set = false;
}

Mesh::~Mesh() {
}

const Domain& Mesh::getDomain() const {
    return *domain;
}

bool Mesh::isSet() const {
    return set;
}

}
