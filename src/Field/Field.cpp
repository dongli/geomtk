#include "Field.h"

namespace geomtk {

Field::Field(const Mesh &mesh, bool hasHalfLevel) {
    this->mesh = &mesh;
}

Field::~Field() {
}

const Mesh& Field::getMesh() const {
    return *mesh;
}

}
