#ifndef __GEOMTK_Velocity__
#define __GEOMTK_Velocity__

#include "geomtk_commons.h"

namespace geomtk {

/**
 *  This class describtes the velocity for the Cartesian domain.
 */
class Velocity {
protected:
    vec v;
public:
    Velocity();
    Velocity(int numDim);
    virtual ~Velocity();

    virtual void
    setNumDim(int numDim);

    double
    operator()(int i) const {
        return v(i);
    }

    double&
    operator()(int i) {
        return v(i);
    }

    const vec&
    operator()() const {
        return v;
    }

    vec& operator()() {
        return v;
    }

    const Velocity
    operator+(const Velocity &other) const;

    const Velocity
    operator-(const Velocity &other) const;

    const Velocity
    operator*(double scale) const;

    const Velocity
    operator/(double scale) const;

    virtual void
    print() const;
}; // Velocity

} // geomtk

#endif // __GEOMTK_Velocity__