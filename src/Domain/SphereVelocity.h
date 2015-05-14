#ifndef __GEOMTK_SphereVelocity__
#define __GEOMTK_SphereVelocity__

#include "Velocity.h"
#include "SphereCoord.h"

namespace geomtk {

class SphereVelocity : public Velocity {
protected:
    vec::fixed<2> vt;
public:
    SphereVelocity();
    SphereVelocity(int numDim);
    virtual ~SphereVelocity();

    const vec::fixed<2>&
    psVelocity() const {
        return vt;
    }

    vec::fixed<2>&
    psVelocity() {
        return vt;
    }

    SphereVelocity&
    operator=(const SphereVelocity &other);

    const SphereVelocity
    operator+(const SphereVelocity &other) const;

    const SphereVelocity
    operator-(const SphereVelocity &other) const;

    const SphereVelocity
    operator*(double scale) const;

    const SphereVelocity
    operator/(double scale) const;

    /**
     * Transform velocity onto polar stereographic plane.
     * @param x the space coordinate.
     */
    void
    transformToPS(const SphereCoord &x);

    /**
     * Transform velocity onto polar stereographic plane.
     * Incorperate calculated sine and cosine of longitude and latitude of
     * grids.
     * @param sinLat 
     * @param sinLat2
     * @param sinLon
     * @param cosLon
     */
    void
    transformToPS(double sinLat, double sinLat2, double sinLon, double cosLon);

    /**
     * Transform velocity from polar stereographic plane.
     * @param x the space coordinate.
     */
     void
    transformFromPS(const SphereCoord &x);

    virtual void
    print() const;
}; // SphereVelocity

} // geomtk

#endif // __GEOMTK_SphereVelocity__