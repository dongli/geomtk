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
    SphereVelocity &y, RLLMeshIndex *idx_) {
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = idx_;
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
            y(2) = c*f(2)(timeIdx, i, j, k)+(1.0-c)*f(2)(timeIdx, i, j, k+1);
        }
        if (!idx->isMoveOnPole()) {
            y.transformFromPS(x);
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            for (int m = 0; m < 2; ++m) {
                i1 = (*idx)(0, f(m).gridType(0));
                i2 = i1+1; i3 = i1; i4 = i2;
                j1 = (*idx)(1, f(m).gridType(1));
                j2 = j1; j3 = j1+1; j4 = j3;
                double x1 = mesh().gridCoordComp(0, f(m).gridType(0), i1);
                double x2 = mesh().gridCoordComp(0, f(m).gridType(0), i2);
                double X = (x(0)-x1)/(x2-x1);
                double y1 = mesh().gridCoordComp(1, f(m).gridType(1), j1);
                double y2 = mesh().gridCoordComp(1, f(m).gridType(1), j3);
                double Y = (x(1)-y1)/(y2-y1);
                double f1 = f(m)(timeIdx, i1, j1, 0);
                double f2 = f(m)(timeIdx, i2, j2, 0);
                double f3 = f(m)(timeIdx, i3, j3, 0);
                double f4 = f(m)(timeIdx, i4, j4, 0);
                double a = f1;
                double b = f2-f1;
                double c = f3-f1;
                double d = f1-f2-f3+f4;
                y(m) = a+b*X+c*Y+d*X*Y;
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (idx->isMoveOnPole()) {
            y.transformToPS(x);
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

}