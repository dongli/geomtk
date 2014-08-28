#ifndef __Geomtk_CartesianDomain__
#define __Geomtk_CartesianDomain__

#include "Domain.h"

namespace geomtk {

class CartesianDomain : public Domain {
public:
	CartesianDomain() : Domain() {}
	CartesianDomain(int numDim) : Domain(numDim) {}
	CartesianDomain(VertCoordType type) : Domain(type) {}
	virtual ~CartesianDomain() {}
};

} // geomtk

#endif // __Geomtk_CartesianDomain__