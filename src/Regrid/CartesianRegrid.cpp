#include "CartesianRegrid.h"

namespace geomtk {

CartesianRegrid::
CartesianRegrid() : Regrid<CartesianMesh, SpaceCoord>() {
}

CartesianRegrid::
CartesianRegrid(const CartesianMesh &mesh)
: Regrid<CartesianMesh, SpaceCoord>(mesh) {
}

CartesianRegrid::~CartesianRegrid() {
}

void CartesianRegrid::
run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
    const CartesianVelocityField &f, const SpaceCoord &x,
    Velocity &y, CartesianMeshIndex *idx_) {
    for (uword m = 0; m < mesh().domain().numDim(); ++m) {
        run(method, timeIdx, f(m), x, y(m), idx_);
    }
}

} // geomtk