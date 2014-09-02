#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"

namespace geomtk {

void CartesianMesh::move(const SpaceCoord &x0, double dt, const Velocity &v,
                         const CartesianMeshIndex &idx, SpaceCoord &x1) const {
    for (int m = 0; m < domain->getNumDim(); ++m) {
        x1(m) = x0(m)+dt*v(m);
        if (domain->getAxisStartBndType(m) == PERIODIC) {
            if (x1(m) < domain->getAxisStart(m)) {
                x1(m) += domain->getAxisSpan(m);
            } else if (x1(m) > domain->getAxisEnd(m)) {
                x1(m) -= domain->getAxisSpan(m);
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}

}