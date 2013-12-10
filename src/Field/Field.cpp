#include "Field.h"

namespace geomtk {

Field::Field(Mesh &mesh) {
    this->mesh = &mesh;
}

Field::~Field() {
}

const Mesh& Field::getMesh() const {
    return *mesh;
}

}
