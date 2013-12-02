#ifndef __RLLRegrid__
#define __RLLRegrid__

#include "Regrid.h"
class RLLVelocityField;
class SphereVelocity;

class RLLRegrid : public Regrid {
protected:
public:
    RLLRegrid(Mesh &mesh);
    virtual ~RLLRegrid();

    virtual void run(RegridMethod method, int timeLevel, const Field &f,
                     const SpaceCoord &x, double &y, MeshIndex *idx = NULL);
    virtual void run(RegridMethod method, int timelevel, const Field &f,
                     const SpaceCoord &x, vec &y, MeshIndex *idx = NULL);
    /**
     * @brief Interpolate velocity field.
     * Velocity field is special, because its direction is singular on Poles.
     */
    virtual void run(RegridMethod method, int timeLevel, const RLLVelocityField &f,
                     const SpaceCoord &x, SphereVelocity &y);
};

#endif
