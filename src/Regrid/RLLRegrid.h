#ifndef __RLLRegrid__
#define __RLLRegrid__

#include "Regrid.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "RLLVelocityField.h"
#include "RLLTensorField.h"

namespace geomtk {

class RLLRegrid : public Regrid {
protected:
public:
    RLLRegrid(const Mesh &mesh);
    virtual ~RLLRegrid();

    virtual void run(RegridMethod method, int timeLevel, const Field &f,
                     const SpaceCoord &x, double &y, MeshIndex *idx = NULL);
    virtual void run(RegridMethod method, int timelevel, const Field &f,
                     const SpaceCoord &x, vec &y, MeshIndex *idx = NULL);

    /**
     *  Interpolate velocity field onto the given coordinate.
     *
     *  The velocity is special, because its direction is singular on Poles.
     *
     *  @param method    the interpolation method (see RegridMethod enum).
     *  @param timeLevel the time level.
     *  @param f         the velocity field.
     *  @param x         the coordinate.
     *  @param y         the output velocity.
     *  @param idx       the mesh index of 'x' (optional).
     *
     *  @return None.
     */
    void run(RegridMethod method, int timeLevel, const RLLVelocityField &f,
             const SpaceCoord &x, SphereVelocity &y, RLLMeshIndex *idx = NULL);

    /**
     *  Interpolate velocity tensor field onto the given coordinate.
     *
     *  @param method    the interpolation method (see RegridMethod enum).
     *  @param timeLevel the time level.
     *  @param f         the velocity gradient tensor field.
     *  @param x         the coordinate.
     *  @param y         the output tensor.
     *  @param idx       the mesh index of 'x' (optional).
     *
     *  @return None.
     */
    void run(RegridMethod method, int timeLevel, const RLLTensorField &f,
             const SpaceCoord &x, mat &y, RLLMeshIndex *idx = NULL);
};

}

#endif
