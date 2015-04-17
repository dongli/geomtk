#include "CartesianRegrid.h"

namespace geomtk {

CartesianRegrid::
CartesianRegrid() : Regrid<CartesianMesh, SpaceCoord>() {
}

CartesianRegrid::
CartesianRegrid(const CartesianMesh &mesh) : Regrid<CartesianMesh, SpaceCoord>(mesh) {
}

CartesianRegrid::~CartesianRegrid() {
}

void CartesianRegrid::
run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
    const CartesianVelocityField &f, const SpaceCoord &x,
    Velocity &y, CartesianMeshIndex *idx_) {
    CartesianMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new CartesianMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = idx_;
    }
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
}

} // geomtk