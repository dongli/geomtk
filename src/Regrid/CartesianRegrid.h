#ifndef __Geomtk_CartesianRegrid__
#define __Geomtk_CartesianRegrid__

#include "Regrid.h"
#include "CartesianMeshIndex.h"
#include "CartesianField.h"

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
};

template <typename T, int N>
void CartesianRegrid::run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
                           const CartesianField<T, N> &f, const SpaceCoord &x,
                           T &y, CartesianMeshIndex *idx_) {
    CartesianMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new CartesianMeshIndex(mesh->getDomain().getNumDim());
        idx->locate(*mesh, x);
    } else {
        idx = idx_;
    }
    if (method == BILINEAR) {
        int i1, i2, i3, i4, j1, j2, j3, j4;
        i1 = (*idx)(0, f.getGridType(0));
        i2 = i1+1; i3 = i1; i4 = i2;
        j1 = (*idx)(1, f.getGridType(1));
        j2 = j1; j3 = j1+1; j4 = j3;
        if (f.getGridType(1) == GridType::HALF &&
            (j1 == -1 || j3 == mesh->getNumGrid(1, GridType::HALF))) {
            REPORT_ERROR("Check this error!");
        }
        double x1 = mesh->getGridCoordComp(0, f.getGridType(0), i1);
        double x2 = mesh->getGridCoordComp(0, f.getGridType(0), i2);
        double X = (x(0)-x1)/(x2-x1);
        double y1 = mesh->getGridCoordComp(1, f.getGridType(1), j1);
        double y2 = mesh->getGridCoordComp(1, f.getGridType(1), j3);
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
    } else if (method == TRILINEAR) {
        REPORT_ERROR("Under construction!");
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

} // geomtk

#endif // __Geomtk_CartesianRegrid__
