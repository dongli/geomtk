#include "RLLRegrid.h"

namespace geomtk {

RLLRegrid::RLLRegrid(const Mesh &mesh) : Regrid(mesh) {
}

RLLRegrid::~RLLRegrid() {
}

void RLLRegrid::run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
                    const RLLVelocityField &f, const SpaceCoord &x,
                    SphereVelocity &y, RLLMeshIndex *idx_) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(mesh, x);
    } else {
        idx = idx_;
    }
    if (idx->isInPolarCap()) {
        const SphereDomain &domain = static_cast<const SphereDomain&>(mesh.getDomain());
        const PolarRing &ring = f.getPolarRing(idx->getPole());
        const double eps = 1.0e-10;
        int j = idx->getPole() == SOUTH_POLE ? 1 : mesh.getNumGrid(1, CENTER)-2;
        int k = (*idx)(2, CENTER);
        double sinLat = mesh.getSinLat(CENTER, j);
        double cosLat = mesh.getCosLat(CENTER, j);
        // horizontal velocity
        y[0] = 0.0; y[1] = 0.0;
        bool match = false;
        double ws = 0.0;
        for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
            double d = domain.calcDistance(x, mesh.getGridCoordComp(0, CENTER, i),
                                           sinLat, cosLat);
            if (d < eps) {
                y[0] = ring.getTransformedData(0, timeIdx, i, k);
                y[1] = ring.getTransformedData(1, timeIdx, i, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
                ws += w;
                y[0] += w*ring.getTransformedData(0, timeIdx, i, k);
                y[1] += w*ring.getTransformedData(1, timeIdx, i, k);
            }
        }
        if (!match) {
            y[0] /= ws;
            y[1] /= ws;
        }
        // vertical velocity
        if (domain.getNumDim() == 3) {
            REPORT_ERROR("Under construction!");
            // linear interpolation
            int i = (*idx)(0, CENTER);
            double z1 = mesh.getGridCoordComp(2, EDGE, k);
            double z2 = mesh.getGridCoordComp(2, EDGE, k+1);
            double c = (z2-x(2))/(z2-z1);
            y(2) = c*f(2, timeIdx, i, j, k)+(1.0-c)*f(2, timeIdx, i, j, k+1);
        }
        if (!idx->isMoveOnPole()) {
            y.transformFromPS(x);
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            for (int m = 0; m < 2; ++m) {
                i1 = (*idx)(0, f.getStaggerType(m, 0));
                i2 = i1+1; i3 = i1; i4 = i2;
                j1 = (*idx)(1, f.getStaggerType(m, 1));
                j2 = j1; j3 = j1+1; j4 = j3;
                double x1 = mesh.getGridCoordComp(0, f.getStaggerType(m, 0), i1);
                double x2 = mesh.getGridCoordComp(0, f.getStaggerType(m, 0), i2);
                double X = (x(0)-x1)/(x2-x1);
                double y1 = mesh.getGridCoordComp(1, f.getStaggerType(m, 1), j1);
                double y2 = mesh.getGridCoordComp(1, f.getStaggerType(m, 1), j3);
                double Y = (x(1)-y1)/(y2-y1);
                double f1 = f(m, timeIdx, i1, j1, 0);
                double f2 = f(m, timeIdx, i2, j2, 0);
                double f3 = f(m, timeIdx, i3, j3, 0);
                double f4 = f(m, timeIdx, i4, j4, 0);
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