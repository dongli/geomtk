#include "Regrid.h"

namespace geomtk {

Regrid::Regrid(const Mesh &mesh) {
    this->mesh = &mesh;
}

Regrid::~Regrid() {
}

}
