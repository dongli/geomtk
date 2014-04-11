#ifndef __Geomtk_RLLRegrid__
#define __Geomtk_RLLRegrid__

#include "Regrid.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "RLLField.h"
#include "RLLVelocityField.h"

namespace geomtk {

class RLLRegrid : public Regrid {
public:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
public:
    RLLRegrid(const Mesh &mesh);
    virtual ~RLLRegrid();

    template <typename T, int N>
    void run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
             const RLLField<T, N> &f, const SphereCoord &x, T &y,
             RLLMeshIndex *idx = NULL);

    template <typename T, int N>
    void run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
             const NumericRLLField<T, N> &f, const SphereCoord &x, T &y,
             RLLMeshIndex *idx = NULL);

    void run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
             const RLLVelocityField &f, const SphereCoord &x, SphereVelocity &y,
             RLLMeshIndex *idx = NULL);
};

// -----------------------------------------------------------------------------

template <typename T, int N>
void RLLRegrid::run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
                    const RLLField<T, N> &f, const SphereCoord &x, T &y,
                    RLLMeshIndex *idx_) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(*(this->mesh), x);
    } else {
        idx = idx_;
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation
        REPORT_ERROR("Under construction!");
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, f.getGridType(0));
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, f.getGridType(1));
            j2 = j1; j3 = j1+1; j4 = j3;
            if (f.getGridType(1) == GridType::HALF &&
                (j1 == -1 || j3 == mesh.getNumGrid(1, GridType::HALF))) {
                REPORT_ERROR("Check this error!");
            }
            double x1 = mesh.getGridCoordComp(0, f.getGridType(0), i1);
            double x2 = mesh.getGridCoordComp(0, f.getGridType(0), i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh.getGridCoordComp(1, f.getGridType(1), j1);
            double y2 = mesh.getGridCoordComp(1, f.getGridType(1), j3);
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
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

template <typename T, int N>
void RLLRegrid::run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
                    const NumericRLLField<T, N> &f, const SphereCoord &x, T &y,
                    RLLMeshIndex *idx_) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(*(this->mesh), x);
    } else {
        idx = idx_;
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation
        const SphereDomain &domain = static_cast<const SphereDomain&>(mesh.getDomain());
        const double eps = 1.0e-10;
        int j = idx->getPole() == SOUTH_POLE ? 1 : mesh.getNumGrid(1, RLLStagger::GridType::FULL)-2;
        int k = (*idx)(2, RLLStagger::GridType::FULL);
        double sinLat = mesh.getSinLat(RLLStagger::GridType::FULL, j);
        double cosLat = mesh.getCosLat(RLLStagger::GridType::FULL, j);
        // horizontal velocity
        y = 0;
        bool match = false;
        double ws = 0.0;
        for (int i = 0; i < mesh.getNumGrid(0, RLLStagger::GridType::FULL); ++i) {
            double d = domain.calcDistance(x, mesh.getGridCoordComp(0, RLLStagger::GridType::FULL, i),
                                           sinLat, cosLat);
            if (d < eps) {
                y = f(timeIdx, i, j, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
                ws += w;
                y += w*f(timeIdx, i, j, k);
            }
        }
        if (!match) {
            y /= ws;
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, f.getGridType(0));
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, f.getGridType(1));
            j2 = j1; j3 = j1+1; j4 = j3;
            if (f.getGridType(1) == GridType::HALF &&
                (j1 == -1 || j3 == mesh.getNumGrid(1, GridType::HALF))) {
                REPORT_ERROR("Check this error!");
            }
            double x1 = mesh.getGridCoordComp(0, f.getGridType(0), i1);
            double x2 = mesh.getGridCoordComp(0, f.getGridType(0), i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh.getGridCoordComp(1, f.getGridType(1), j1);
            double y2 = mesh.getGridCoordComp(1, f.getGridType(1), j3);
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
    }
    if (idx_ == NULL) {
        delete idx;
    }
}
    
}

#endif
