#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"

namespace geomtk {

void CartesianMesh::setCellVolumes() {
    volumes.set_size(numGrid(0, GridType::FULL),
                     numGrid(1, GridType::FULL),
                     numGrid(2, GridType::FULL));
    if (domain().numDim() == 1) {
        for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
            int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
            double dx = gridInterval(0, GridType::HALF, I);
            volumes(I) = dx;
        }
    } else if (domain().numDim() == 2) {
        for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
            int J = gridStyles[1] == FULL_LEAD ? j-haloWidth() : j;
            double dy = gridInterval(1, GridType::HALF, J);
            for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
                double dx = gridInterval(0, GridType::HALF, I);
                volumes(I, J) = dx*dy;
            }
        }
    } else if (domain().numDim() == 3) {
        for (int k = ks(GridType::FULL); k <= ke(GridType::FULL); ++k) {
            int K = gridStyles[2] == FULL_LEAD ? k-haloWidth() : k;
            double dz = gridInterval(2, GridType::HALF, K);
            for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
                int J = gridStyles[1] == FULL_LEAD ? j-haloWidth() : j;
                double dy = gridInterval(1, GridType::HALF, J);
                for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                    int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
                    double dx = gridInterval(0, GridType::HALF, I);
                    volumes(I, J, K) = dx*dy*dz;
                }
            }
        }
    }
#ifndef NDEBUG
    double totalVolume = 0.0, diff;
    for (int i = 0; i < totalNumGrid(Location::CENTER, domain().numDim()); ++i) {
        totalVolume += volumes(i);
    }
    if (domain().numDim() == 1) {
        diff = fabs(totalVolume-domain().axisSpan(0));
    } else if (domain().numDim() == 2) {
        diff = fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1));
    } else if (domain().numDim() == 3) {
        diff = fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1)*domain().axisSpan(2));
    }
    if (diff > 1.0e-10) {
        REPORT_ERROR("Total volume difference is " << diff << ", and it is too large!");
    }
#endif
}

void CartesianMesh::move(const SpaceCoord &x0, double dt, const Velocity &v,
                         const CartesianMeshIndex &idx, SpaceCoord &x1) const {
    x1() = x0()+dt*v();
    domain().constrain(x1);
}

}