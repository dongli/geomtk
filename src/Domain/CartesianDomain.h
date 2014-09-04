#ifndef __Geomtk_CartesianDomain__
#define __Geomtk_CartesianDomain__

#include "Domain.h"

namespace geomtk {

class CartesianDomain : public Domain<SpaceCoord> {
public:
	CartesianDomain() : Domain() {}
	CartesianDomain(int numDim) : Domain(numDim) {}
	CartesianDomain(VertCoordType type) : Domain(type) {}
	virtual ~CartesianDomain() {}

    virtual vec diffCoord(const SpaceCoord &x, const SpaceCoord &y) const;

    virtual double calcDistance(const SpaceCoord &x, const SpaceCoord &y) const;
};

} // geomtk

#endif // __Geomtk_CartesianDomain__