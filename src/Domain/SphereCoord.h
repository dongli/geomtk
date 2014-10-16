#ifndef __Geomtk_SphereCoord__
#define __Geomtk_SphereCoord__

#include "SpaceCoord.h"

namespace geomtk {

class SphereDomain;

enum Pole {
    SOUTH_POLE = 0, NORTH_POLE = 1, NOT_POLE = 2
};

/**
 *  This class describes the coordinate of sphere.
 */

class SphereCoord : public SpaceCoord {
protected:
    vec xt; //>! horizontal coordinate on polar stereographic plane
    // TODO: How to calculate Cartesian coordinate, since the vertical
    //       coordinate may not be height?
    vec::fixed<3> _cartCoord; //>! Cartesian coordinate representation
    double _cosLon, _sinLon, _cosLat, _sinLat;
public:
    SphereCoord();
    SphereCoord(int numDim);
    SphereCoord(const SphereCoord &other);
    virtual ~SphereCoord();

    virtual void setNumDim(int numDim);

    virtual void setCoord(double lon, double lat);

    virtual void setCoord(double lon, double lat, double lev);

    virtual void setCoordComp(int dim, double comp);

    void setCartCoord(double x, double y, double z);

    void updateTrigonometricFunctions();

    double cosLon() const { return _cosLon; }

    double sinLon() const { return _sinLon; }

    double cosLat() const { return _cosLat; }

    double sinLat() const { return _sinLat; }

    const vec& psCoord() const { return xt; }

    vec& psCoord() { return xt; }

    SphereCoord& operator=(const SphereCoord& other);

    void transformToPS(const SphereDomain &domain);

    void transformFromPS(const SphereDomain &domain, Pole pole);

    void transformToCart(const SphereDomain &domain);

    virtual const vec& cartCoord() const { return _cartCoord; }

    virtual void print() const;
};

} // geomtk

#endif // __Geomtk_SphereCoord__