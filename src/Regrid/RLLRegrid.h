#ifndef __Geomtk_RLLRegrid__
#define __Geomtk_RLLRegrid__

#include "Regrid.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "RLLField.h"
#include "RLLVelocityField.h"

namespace geomtk {

class RLLRegrid : public Regrid {
protected:
public:
    RLLRegrid(const Mesh &mesh);
    virtual ~RLLRegrid();

    template <typename T>
    void run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
             const RLLField<T> &f, const SpaceCoord &x, T &y,
             RLLMeshIndex *idx = NULL);

    void run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
             const RLLVelocityField &f, const SpaceCoord &x, SphereVelocity &y,
             RLLMeshIndex *idx = NULL);
};

// -----------------------------------------------------------------------------

template <typename T>
void RLLRegrid::run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
                    const RLLField<T> &f, const SpaceCoord &x, T &y,
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
        
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, f.getStaggerType(0));
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, f.getStaggerType(1));
            j2 = j1; j3 = j1+1; j4 = j3;
            double x1 = mesh.getGridCoord(0, f.getStaggerType(0), i1);
            double x2 = mesh.getGridCoord(0, f.getStaggerType(0), i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh.getGridCoord(1, f.getStaggerType(1), j1);
            double y2 = mesh.getGridCoord(1, f.getStaggerType(1), j3);
            double Y = (x(1)-y1)/(y2-y1);
            T f1 = f(timeIdx, i1, j1);
            T f2 = f(timeIdx, i2, j2);
            T f3 = f(timeIdx, i3, j3);
            T f4 = f(timeIdx, i4, j4);
            T a = f1;
            T b = f2-f1;
            T c = f3-f1;
            T d = f1-f2-f3+f4;
            y = a+b*X+c*Y+d*X*Y;
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}
    
}

#endif
