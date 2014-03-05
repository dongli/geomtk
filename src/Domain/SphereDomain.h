#ifndef __SphereDomain__
#define __SphereDomain__

#include "Domain.h"

namespace geomtk {

enum Pole {
    SOUTH_POLE = 0, NORTH_POLE = 1, NOT_POLE = 2
};

class SphereDomain;

/**
 * SphereCoord
 * This class describes the coordinate of sphere.
 */

class SphereCoord : public SpaceCoord {
protected:
    vec xt; //>! horizontal coordinate on polar stereographic plane
    // TODO: How to calculate Cartesian coordinate, since the vertical
    //       coordinate may not be height?
    vec::fixed<3> cartCoord; //>! Cartesian coordinate representation
    double cosLon, sinLon, cosLat, sinLat;
public:
    SphereCoord(int numDim);
    SphereCoord(const SphereCoord &other);
    virtual ~SphereCoord();

    void setCoord(double lon, double lat);
    void setCoordComp(int dim, double comp);
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

    void transformFromPS(const SphereDomain &domain);
    void transformFromPS(const SphereDomain &domain, Pole pole);
    
    void transformToCart(const SphereDomain &domain);
    
    const vec& getCartCoord() const { return cartCoord; }

    virtual void print() const;
};

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

class SphereDomain : public Domain {
    double radius;
public:
    enum ProjectionType {
        STEREOGRAPHIC
    };
    
    SphereDomain();
    SphereDomain(int numDim);
    virtual ~SphereDomain();

    double getRadius() const { return radius; }
    void setRadius(double radius) { this->radius = radius; }

    double calcDistance(const SphereCoord &x, const SphereCoord &y) const;
    double calcDistance(const SphereCoord &x, double lon, double lat) const;
    double calcDistance(const SphereCoord &x, double lon, double sinLat,
                        double cosLat) const;

    virtual vec diffCoord(const SphereCoord &x, const SphereCoord &y) const;

    /**
     *  Transform the given space coordinate 'xo' into a rotated spherical
     *  coordinate system with North Pole coordinate in original one as 'xp'.
     *  The output is put in 'xr'
     *
     *  @param xp the rotated North Pole coordinate.
     *  @param xo the original space coordinate.
     *  @param xr the transformed space coordinate.
     */
    void rotate(const SphereCoord &xp, const SphereCoord &xo,
                SphereCoord &xr) const;

    /**
     *  Transform the rotated coordinate back to original spherical coordinate
     *  system.
     *
     *  @param xp the rotated North Pole coordinate.
     *  @param xo the original space coordinate.
     *  @param xr the transformed space coordinate.
     */
    void rotateBack(const SphereCoord &xp, SphereCoord &xo,
                    const SphereCoord &xr) const;

    void project(ProjectionType projType, const SphereCoord &xp,
                 const SphereCoord &xo, vec &xs) const;
    
    void projectBack(ProjectionType projType, const SphereCoord &xp,
                     SphereCoord &xo, const vec &xs) const;

    virtual string getBrief() const;
};

}

#endif
