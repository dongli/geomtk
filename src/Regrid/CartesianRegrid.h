#ifndef __Geomtk_CartesianRegrid__
#define __Geomtk_CartesianRegrid__

#include "Regrid.h"
#include "CartesianMeshIndex.h"
#include "CartesianField.h"
#include "CartesianVelocityField.h"

namespace geomtk {

class CartesianRegrid : public Regrid<CartesianMesh, SpaceCoord> {
public:
    typedef CartesianStagger::GridType GridType;
    typedef CartesianStagger::Location Location;
protected:
public:
    CartesianRegrid(const CartesianMesh &mesh);
    virtual ~CartesianRegrid();

    template <typename T, int N>
    void run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
             const CartesianField<T, N> &f, const SpaceCoord &x, T &y,
             CartesianMeshIndex *idx = NULL);

    void run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
             const CartesianVelocityField &f, const SpaceCoord &x,
             Velocity &v, CartesianMeshIndex *idx = NULL);
};

template <typename T, int N>
void CartesianRegrid::run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
                           const CartesianField<T, N> &f, const SpaceCoord &x,
                           T &y, CartesianMeshIndex *idx_) {
    CartesianMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new CartesianMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = idx_;
    }
    if (method == BILINEAR) {
#ifndef NDEBUG
        assert(mesh().domain().numDim() == 2);
#endif
        int i1, i2, i3, i4, j1, j2, j3, j4;
        i1 = (*idx)(0, f.gridType(0));
        i2 = i1+1; i3 = i1; i4 = i2;
        j1 = (*idx)(1, f.gridType(1));
        j2 = j1; j3 = j1+1; j4 = j3;
        if (f.gridType(1) == GridType::HALF &&
            (j1 == -1 || j3 == mesh().numGrid(1, GridType::HALF))) {
            REPORT_ERROR("Check this error!");
        }
        double x1 = mesh().gridCoordComp(0, f.gridType(0), i1);
        double x2 = mesh().gridCoordComp(0, f.gridType(0), i2);
        double X = (x(0)-x1)/(x2-x1);
        double y1 = mesh().gridCoordComp(1, f.gridType(1), j1);
        double y2 = mesh().gridCoordComp(1, f.gridType(1), j3);
        double Y = (x(1)-y1)/(y2-y1);
#ifndef NDEBUG
        assert(X >= 0.0 && X <= 1.0 && Y >= 0.0 && Y <= 1.0);
#endif
        double f1 = f(timeIdx, i1, j1);
        double f2 = f(timeIdx, i2, j2);
        double f3 = f(timeIdx, i3, j3);
        double f4 = f(timeIdx, i4, j4);
        double a = f1;
        double b = f2-f1;
        double c = f3-f1;
        double d = f1-f2-f3+f4;
        y = a+b*X+c*Y+d*X*Y;
    } else if (method == BIQUADRATIC || method == BICUBIC) {
        // TODO: Merge the above BILINEAR into here.
        // Use Lagrangian polynomial interpolation without derivatives.
#ifndef NDEBUG
        assert(mesh().domain().numDim() == 2);
#endif
        int n;
        if (method == BIQUADRATIC) {
            n = 3;
        } else if (method == BICUBIC) {
            n = 4;
        }
        int i[n], j[n];
        i[0] = (*idx)(0, f.gridType(0))-n/2+1;
        j[0] = (*idx)(1, f.gridType(1))-n/2+1;
        for (int l = 1; l < n; ++l) {
            i[l] = i[l-1]+1;
            j[l] = j[l-1]+1;
        }
#ifndef NDEBUG
        for (int m = 0; m < 2; ++m) {
            if (mesh().domain().axisStartBndType(m) != PERIODIC &&
                (i[0] == mesh().is(f.gridType(m))-1 ||
                 i[n-1] == mesh().ie(f.gridType(m))+1)) {
                idx->print();
                REPORT_ERROR("Point is out of range!");
            }
        }
#endif
        double wx[n], wy[n];
        for (int l0 = 0; l0 < n; ++l0) {
            double x0 = mesh().gridCoordComp(0, f.gridType(0), i[l0]);
            double y0 = mesh().gridCoordComp(1, f.gridType(1), j[l0]);
            wx[l0] = 1; wy[l0] = 1;
            for (int l1 = 0; l1 < n; ++l1) {
                if (l0 != l1) {
                    double x1 = mesh().gridCoordComp(0, f.gridType(0), i[l1]);
                    double y1 = mesh().gridCoordComp(1, f.gridType(1), j[l1]);
                    wx[l0] *= (x(0)-x1)/(x0-x1);
                    wy[l0] *= (x(1)-y1)/(y0-y1);
                }
            }
        }
        y = 0;
        for (int l0 = 0; l0 < n; ++l0) {
            for (int l1 = 0; l1 < n; ++l1) {
                y += wx[l0]*wy[l1]*f(timeIdx, i[l0], j[l1]);
            }
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

} // geomtk

#endif // __Geomtk_CartesianRegrid__
