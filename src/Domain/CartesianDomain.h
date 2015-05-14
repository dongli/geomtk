#ifndef __GEOMTK_CartesianDomain__
#define __GEOMTK_CartesianDomain__

#include "Domain.h"

namespace geomtk {

class CartesianDomain
: public Domain<SpaceCoord> {
public:
	CartesianDomain() : Domain() {}
	CartesianDomain(int numDim) : Domain(numDim) {}
	CartesianDomain(VertCoordType type) : Domain(type) {}
	virtual ~CartesianDomain() {}

    virtual void
    init(const string &filePath);

    virtual vec
    diffCoord(const SpaceCoord &x, const SpaceCoord &y) const;

    virtual vec
    diffCoord(const vec &x, const vec &y) const;

    virtual double
    calcDistance(const SpaceCoord &x, const SpaceCoord &y) const;

    virtual double
    calcDistance(const vec &x, const vec &y) const;
}; // CartesianDomain

} // geomtk

#endif // __GEOMTK_CartesianDomain__