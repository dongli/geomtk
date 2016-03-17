#include "RLLRegrid.h"

namespace geomtk {

RLLRegrid::
RLLRegrid() : Regrid<RLLMesh, SphereCoord>() {
}

RLLRegrid::
RLLRegrid(const RLLMesh &mesh) : Regrid<RLLMesh, SphereCoord>(mesh) {
}

RLLRegrid::
~RLLRegrid() {
}

void RLLRegrid::
run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
    const RLLVelocityField &f, const SphereCoord &x,
    SphereVelocity &y, RLLMeshIndex *_idx) {
    RLLMeshIndex *idx;
    if (_idx == NULL) {
        idx = new RLLMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = _idx;
    }
    if (idx->isInPolarCap()) {
        const SphereDomain &domain = mesh().domain();
        const PolarRing &ring = f.getPolarRing(idx->pole());
        const double eps = 1.0e-10;
        int j = idx->pole() == SOUTH_POLE ? 1 : mesh().numGrid(1, RLLStagger::GridType::FULL)-2;
        int k = (*idx)(2, RLLStagger::GridType::FULL);
        double sinLat = mesh().sinLat(RLLStagger::GridType::FULL, j);
        double cosLat = mesh().cosLat(RLLStagger::GridType::FULL, j);
        // horizontal velocity
        y.psVelocity()[0] = 0.0;
        y.psVelocity()[1] = 0.0;
        bool match = false;
        double W = 0.0;
        for (uword i = mesh().is(RLLStagger::GridType::FULL);
             i <= mesh().ie(RLLStagger::GridType::FULL); ++i) {
            double d = domain.calcDistance(x, mesh().gridCoordComp(0, RLLStagger::GridType::FULL, i),
                                           sinLat, cosLat);
            if (d < eps) {
                y.psVelocity()[0] = ring.transformedData(0, timeIdx, i, k);
                y.psVelocity()[1] = ring.transformedData(1, timeIdx, i, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
                W += w;
                y.psVelocity()[0] += w*ring.transformedData(0, timeIdx, i, k);
                y.psVelocity()[1] += w*ring.transformedData(1, timeIdx, i, k);
            }
        }
        if (!match) {
            y.psVelocity()[0] /= W;
            y.psVelocity()[1] /= W;
        }
        // vertical velocity
        if (domain.numDim() == 3) {
            REPORT_ERROR("Under construction!");
            // linear interpolation
            int i = (*idx)(0, RLLStagger::GridType::FULL);
            double z1 = mesh().gridCoordComp(2, RLLStagger::GridType::HALF, k);
            double z2 = mesh().gridCoordComp(2, RLLStagger::GridType::HALF, k+1);
            double c = (z2-x(2))/(z2-z1);
            y(2) = c*f(2)(timeIdx)(i, j, k)+(1.0-c)*f(2)(timeIdx)(i, j, k+1);
        }
        if (!idx->isMoveOnPole()) {
            y.transformFromPS(x);
        }
    } else {
        for (int m = 0; m < mesh().domain().numDim(); ++m) {
            run(method, timeIdx, f(m), x, y(m), idx);
        }
        if (idx->isMoveOnPole()) {
            y.transformToPS(x);
        }
    }
    if (_idx == NULL) {
        delete idx;
    }
}

}
