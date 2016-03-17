#ifndef __GEOMTK_CartesianRegrid__
#define __GEOMTK_CartesianRegrid__

#include "Regrid.h"
#include "CartesianMeshIndex.h"
#include "CartesianField.h"
#include "CartesianVelocityField.h"

namespace geomtk {

class CartesianRegrid : public Regrid<CartesianMesh, SpaceCoord> {
public:
    typedef CartesianStagger::GridType GridType;
    typedef CartesianStagger::Location Location;
public:
    CartesianRegrid();
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
void CartesianRegrid::
run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
    const CartesianField<T, N> &f, const SpaceCoord &x,
    T &y, CartesianMeshIndex *_idx) {
    CartesianMeshIndex *idx;
    if (_idx == NULL) {
        idx = new CartesianMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = _idx;
    }
    if (method == LINEAR || method == QUADRATIC || method == CUBIC) {
        int n = 0;
        if (method == LINEAR) {
            n = 2;
        } else if (method == QUADRATIC) {
            n = 3;
        } else if (method == CUBIC) {
            n = 4;
        }
        int n1 = n/2-2;
        int n2 = -(n-1)/2;
        int i[mesh().domain().numDim()][n];
        for (uword m = 0; m < mesh().domain().numDim(); ++m) {
            i[m][0] = (*idx)(m, f.gridType(m))-n/2+1;
            if (mesh().domain().axisStartBndType(m) != PERIODIC) {
                if ((*idx)(m, f.gridType(m)) == static_cast<int>(mesh().startIndex(m, f.gridType(m)))+n1) {
                    i[m][0]++;
                } else if ((*idx)(m, f.gridType(m)) == static_cast<int>(mesh().endIndex(m, f.gridType(m)))+n2) {
                    i[m][0]--;
                }
            } else {
                if (i[m][0] < 0) {
                    REPORT_ERROR("The halo width is not sufficient for the interpolation!");
                } else if (i[m][0]+n > static_cast<int>(mesh().numGrid(m, f.gridType(m), true))) {
                    REPORT_ERROR("The halo width is not sufficient for the interpolation!");
                }
            }
            for (int l = 1; l < n; ++l) {
                i[m][l] = i[m][l-1]+1;
            }
        }
        double w[mesh().domain().numDim()][n];
        for (uword m = 0; m < mesh().domain().numDim(); ++m) {
            for (int l0 = 0; l0 < n; ++l0) {
                double x0 = mesh().gridCoordComp(m, f.gridType(m), i[m][l0]);
                w[m][l0] = 1;
                for (int l1 = 0; l1 < n; ++l1) {
                    if (l0 == l1) continue;
                    double x1 = mesh().gridCoordComp(m, f.gridType(m), i[m][l1]);
                    w[m][l0] *= (x(m)-x1)/(x0-x1);
                }
            }

        }
        y = 0;
        switch (mesh().domain().numDim()) {
            case 1:
                for (int l = 0; l < n; ++l) {
                    y += w[0][l]*f(timeIdx)(i[0][l]);
                }
                break;
            case 2:
                for (int l0 = 0; l0 < n; ++l0) {
                    for (int l1 = 0; l1 < n; ++l1) {
                        y += w[0][l0]*w[1][l1]*f(timeIdx)(i[0][l0], i[1][l1]);
                    }
                }
                break;
            case 3:
                for (int l0 = 0; l0 < n; ++l0) {
                    for (int l1 = 0; l1 < n; ++l1) {
                        for (int l2 = 0; l2 < n; ++l2) {
                            y += w[0][l0]*w[1][l1]*w[2][l2]*f(timeIdx)(i[0][l0], i[1][l1], i[2][l2]);
                        }
                    }
                }
                break;
            default:
                break;
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
    if (_idx == NULL) {
        delete idx;
    }
} // run

} // geomtk

#endif // __GEOMTK_CartesianRegrid__
