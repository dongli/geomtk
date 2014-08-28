#ifndef __Geomtk_CartesianMesh__
#define __Geomtk_CartesianMesh__

#include "StructuredMesh.h"
#include "CartesianDomain.h"

namespace geomtk {

struct CartesianStagger : public StructuredStagger {
};

class CartesianMesh : public StructuredMesh<CartesianDomain, SpaceCoord> {
public:
public:
    typedef SpaceCoord CoordType;
    typedef CartesianStagger::GridType GridType;
    typedef CartesianStagger::Location Location;

    CartesianMesh(CartesianDomain &domain)
            : StructuredMesh<CartesianDomain, SpaceCoord>(domain) {
        this->type = CARTESIAN_MESH;
    }
    virtual ~CartesianMesh() {}
};

} // geomtk

#endif // __Geomtk_CartesianMesh__
