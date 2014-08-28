#include "CartesianRegrid.h"

namespace geomtk {

CartesianRegrid::CartesianRegrid(const CartesianMesh &mesh) : Regrid<CartesianMesh, SpaceCoord>(mesh) {
}

CartesianRegrid::~CartesianRegrid() {
}

} // geomtk