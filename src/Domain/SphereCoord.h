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
    vec::fixed<3> cartCoord; //>! Cartesian coordinate representation
    double cosLon, sinLon, cosLat, sinLat;
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

    double getCosLon() const { return cosLon; }

    double getSinLon() const { return sinLon; }

    double getCosLat() const { return cosLat; }

    double getSinLat() const { return sinLat; }

    /**
     *  Subscript operator for polar stereographic projected coorinate.
     *
     *  @param i the coordinate component index.
     *
     *  @return The coordinate componnet.
     */
    double operator[](int i) const { return xt(i); }
    double& operator[](int i) { return xt(i); }

    /**
     *  Get the polar stereographic projected coorinate with the vertical
     *  coordinate component unchanged.
     *
     *  @return The polar stereographic projected coorinate.
     */
    const vec& getPSCoord() const { return xt; }
    vec& getPSCoord() { return xt; }

    SphereCoord& operator=(const SphereCoord& other);

    void transformToPS(const SphereDomain &domain);

    void transformFromPS(const SphereDomain &domain, Pole pole);

    void transformToCart(const SphereDomain &domain);

    virtual const vec& getCartCoord() const { return cartCoord; }

    virtual void print() const;
};

} // geomtk

#endif // __Geomtk_SphereCoord__