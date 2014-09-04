#ifndef __Geomtk_CartesianMesh__
#define __Geomtk_CartesianMesh__

#include "StructuredMesh.h"
#include "CartesianDomain.h"

namespace geomtk {

class CartesianMeshIndex;

struct CartesianStagger : public StructuredStagger {
};

class CartesianMesh : public StructuredMesh<CartesianDomain, SpaceCoord> {
public:
    typedef SpaceCoord CoordType;
    typedef CartesianStagger::GridType GridType;
    typedef CartesianStagger::Location Location;
public:
    CartesianMesh(CartesianDomain &domain)
            : StructuredMesh<CartesianDomain, SpaceCoord>(domain) {
        this->type = CARTESIAN_MESH;
    }
    virtual ~CartesianMesh() {}

    virtual void setCellVolumes();

    void move(const SpaceCoord &x0, double dt, const Velocity &v,
              const CartesianMeshIndex &idx, SpaceCoord &x1) const;
};

} // geomtk

#endif // __Geomtk_CartesianMesh__
