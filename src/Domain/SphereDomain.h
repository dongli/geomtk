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
    double xt[2]; // horizontal coordinate on polar stereographic plane
public:
    SphereCoord(int numDim);
    virtual ~SphereCoord();

    double operator[](int i) const;
    double& operator[](int i);

    SphereCoord& operator=(const SphereCoord& other);

    void transformToPS(const SphereDomain &domain);

    void transformFromPS(const SphereDomain &domain);

    virtual void print() const;
};

// -----------------------------------------------------------------------------

class SphereVelocity : public Velocity {
protected:
    double vt[2];
public:
    SphereVelocity();
    SphereVelocity(int numDim);
    virtual ~SphereVelocity();

    /**
     * Subscript operator of transformed velocity.
     * @param dim which dimension of velocity that is needed.
     */
    double operator[](int dim) const;

    /**
     * Subscript operator of transformed velocity.
     * @param dim which dimension of velocity that is needed.
     */
    double& operator[](int dim);

    SphereVelocity& operator=(const SphereVelocity &other);

    const SphereVelocity operator+(const SphereVelocity &other) const;
    const SphereVelocity operator-(const SphereVelocity &other) const;
    const SphereVelocity operator*(double scale) const;
    const SphereVelocity operator/(double scale) const;

    /**
     * Transform velocity onto polar stereographic plane.
     * @param x the space coordinate.
     */
    void transformToPS(const SpaceCoord &x);

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
     void transformFromPS(const SpaceCoord &x);

    virtual void print() const;
};

// -----------------------------------------------------------------------------

class SphereDomain : public Domain {
    double radius;
public:
    SphereDomain();
    SphereDomain(int numDim);
    virtual ~SphereDomain();

    double getRadius() const;
    void setRadius(double radius);

    virtual double calcDistance(const SpaceCoord &x, const SpaceCoord &y) const;
    virtual double calcDistance(const SpaceCoord &x, double lon,
                                double lat) const;
    virtual double calcDistance(const SpaceCoord &x, double lon,
                                double sinLat, double cosLat) const;

    /**
     *  Transform the given space coordinate 'xo' into a rotated spherical
     *  coordinate system with North Pole coordinate in original one as 'xp'.
     *  The output is put in 'xr'
     *
     *  @param xp the rotated North Pole coordinate.
     *  @param xo the given space coordinate.
     *  @param xr the transformed space coordinate.
     *
     *  @return None (in 'xr').
     */
    virtual void rotate(const SpaceCoord &xp, const SpaceCoord &xo,
                        SpaceCoord &xr) const;
    virtual void rotateBack(const SpaceCoord &xp, SpaceCoord &xo,
                            const SpaceCoord &xr) const;

    virtual string getBrief() const;
};

}

#endif
