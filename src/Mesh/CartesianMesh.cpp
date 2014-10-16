#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"

namespace geomtk {

void CartesianMesh::setCellVolumes() {
    volumes.set_size(numGrid(0, GridType::FULL),
                     numGrid(1, GridType::FULL),
                     numGrid(2, GridType::FULL));
    int I, J, K;
    double dx, dy, dz;
    for (int k = ks(GridType::FULL); k <= ke(GridType::FULL); ++k) {
        if (domain().numDim() == 2) {
            K = 0;
            dz = 1.0;
        } else {
            K = gridStyles[2] == FULL_LEAD ? k-1 : k;
            dz = gridInterval(2, GridType::HALF, K);
        }
        for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
            J = gridStyles[1] == FULL_LEAD ? j-1 : j;
            dy = gridInterval(1, GridType::HALF, J);
            for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                I = gridStyles[0] == FULL_LEAD ? i-1 : i;
                dx = gridInterval(0, GridType::HALF, I);
                volumes(I, J, K) = dx*dy*dz;
            }
        }
    }
#ifndef NDEBUG
    double totalVolume = 0.0;
    for (int i = 0; i < totalNumGrid(Location::CENTER, domain().numDim()); ++i) {
        totalVolume += volumes(i);
    }
    if (domain().numDim() == 2) {
        assert(fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1)) < 1.0e-12);
    } else if (domain().numDim() == 3) {
        assert(fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1)*domain().axisSpan(2)) < 1.0e-12);
    }
#endif
}

void CartesianMesh::move(const SpaceCoord &x0, double dt, const Velocity &v,
                         const CartesianMeshIndex &idx, SpaceCoord &x1) const {
    x1() = x0()+dt*v();
    domain().constrain(x1);
}

}