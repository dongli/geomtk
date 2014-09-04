#ifndef __Geomtk_SpaceCoord__
#define __Geomtk_SpaceCoord__

#include "geomtk_commons.h"

namespace geomtk {

/**
 *  This class describes the Cartesian space coordinate of a point.
 */
class SpaceCoord {
protected:
    vec coord;
public:
    SpaceCoord();
    SpaceCoord(int numDim);
    SpaceCoord(const SpaceCoord &other);
    virtual ~SpaceCoord();

    virtual void setNumDim(int numDim);

    virtual void setCoord(double x, double y);

    virtual void setCoord(double x, double y, double z);

    virtual void setCoordComp(int dim, double comp);

    virtual SpaceCoord& operator=(const SpaceCoord &other);

    virtual double operator()(int i) const { return coord(i); }

    virtual double& operator()(int i) { return coord(i); }

    virtual const vec& operator()() const { return coord; }

    virtual vec& operator()() { return coord; }

    /**
     *  This method return the Cartesian coordinate, in this case it is `coord`,
     *  so this method is just for compatible with SphereCoord.
     *
     *  @return The Cartesian coordinate vector.
     */
    virtual const vec& getCartCoord() const { return coord; }

    virtual void print() const;
};

} // geomtk

#endif // __Geomtk_SpaceCoord__
