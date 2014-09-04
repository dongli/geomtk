#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"

namespace geomtk {

void CartesianMesh::setCellVolumes() {
    volumes.set_size(getNumGrid(0, GridType::FULL),
                     getNumGrid(1, GridType::FULL),
                     getNumGrid(2, GridType::FULL));
    int I, J, K;
    double dx, dy, dz;
    for (int k = ks(GridType::FULL); k <= ke(GridType::FULL); ++k) {
        if (domain->getNumDim() == 2) {
            K = 0;
            dz = 1.0;
        } else {
            K = gridStyles[2] == FULL_LEAD ? k-1 : k;
            dz = getGridInterval(2, GridType::HALF, K);
        }
        for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
            J = gridStyles[1] == FULL_LEAD ? j-1 : j;
            dy = getGridInterval(1, GridType::HALF, J);
            for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                I = gridStyles[0] == FULL_LEAD ? i-1 : i;
                dx = getGridInterval(0, GridType::HALF, I);
                volumes(I, J, K) = dx*dy*dz;
            }
        }
    }
#ifndef NDEBUG
    double totalVolume = 0.0;
    for (int i = 0; i < getTotalNumGrid(Location::CENTER, domain->getNumDim()); ++i) {
        totalVolume += volumes(i);
    }
    if (domain->getNumDim() == 2) {
        assert(fabs(totalVolume-domain->getAxisSpan(0)*domain->getAxisSpan(1)) < 1.0e-12);
    } else if (domain->getNumDim() == 3) {
        assert(fabs(totalVolume-domain->getAxisSpan(0)*domain->getAxisSpan(1)*domain->getAxisSpan(2)) < 1.0e-12);
    }
#endif
}

void CartesianMesh::move(const SpaceCoord &x0, double dt, const Velocity &v,
                         const CartesianMeshIndex &idx, SpaceCoord &x1) const {
    x1() = x0()+dt*v();
    domain->constrain(x1);
}

}