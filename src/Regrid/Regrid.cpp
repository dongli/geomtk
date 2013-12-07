#include "Regrid.h"

namespace geomtk {

Regrid::Regrid(Mesh &mesh) {
    this->mesh = &mesh;
}

Regrid::~Regrid() {
}

}
