#ifndef __Geomtk_SphereVelocity__
#define __Geomtk_SphereVelocity__

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

    /**
     * Subscript operator of transformed velocity.
     * @param dim which dimension of velocity that is needed.
     */
    double operator[](int i) const { return vt(i); }

    /**
     * Subscript operator of transformed velocity.
     * @param dim which dimension of velocity that is needed.
     */
    double& operator[](int i) { return vt(i); }

    SphereVelocity& operator=(const SphereVelocity &other);

    const SphereVelocity operator+(const SphereVelocity &other) const;
    const SphereVelocity operator-(const SphereVelocity &other) const;
    const SphereVelocity operator*(double scale) const;
    const SphereVelocity operator/(double scale) const;

    /**
     * Transform velocity onto polar stereographic plane.
     * @param x the space coordinate.
     */
    void transformToPS(const SphereCoord &x);

    /**
     * Transform velocity onto polar stereographic plane.
     * Incorperate calculated sine and cosine of longitude and latitude of
     * grids.
     * @param sinLat 
     * @param sinLat2
     * @param sinLon
     * @param cosLon
     */
    void transformToPS(double sinLat, double sinLat2,
                       double sinLon, double cosLon);

    /**
     * Transform velocity from polar stereographic plane.
     * @param x the space coordinate.
     */
     void transformFromPS(const SphereCoord &x);

    virtual void print() const;
};

} // geomtk

#endif // __Geomtk_SphereVelocity__