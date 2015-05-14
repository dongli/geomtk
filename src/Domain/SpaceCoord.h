#ifndef __GEOMTK_SpaceCoord__
#define __GEOMTK_SpaceCoord__

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

    virtual void
    init(int numDim);

    virtual vec
    data() const {
        return coord;
    }

    virtual void
    set(double x, double y);

    virtual void
    set(double x, double y, double z);

    virtual void
    setComp(int i, double comp);

    virtual SpaceCoord&
    operator=(const SpaceCoord &other);

    virtual double
    operator()(int i) const {
#ifndef NDEBUG
        return coord(i);
#else
        return coord[i];
#endif
    }

    virtual double&
    operator()(int i) {
#ifndef NDEBUG
        return coord(i);
#else
        return coord[i];
#endif
    }

    virtual const vec&
    operator()() const {
        return coord;
    }

    virtual vec&
    operator()() {
        return coord;
    }

    /**
     *  This method return the Cartesian coordinate, in this case it is `coord`,
     *  so this method is just for compatible with SphereCoord.
     *
     *  @return The Cartesian coordinate vector.
     */
    virtual const vec&
    cartCoord() const {
        return coord;
    }

    virtual vec&
    cartCoord() {
        return coord;
    }

    virtual void
    print() const;
}; // SpaceCoord

} // geomtk

#endif // __GEOMTK_SpaceCoord__
