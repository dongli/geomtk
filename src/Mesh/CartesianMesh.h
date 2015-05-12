#ifndef __GEOMTK_CartesianMesh__
#define __GEOMTK_CartesianMesh__

#include "StructuredMesh.h"
#include "CartesianDomain.h"

namespace geomtk {

class CartesianMeshIndex;

struct CartesianStagger : public StructuredStagger {
};

class CartesianMesh : public StructuredMesh<CartesianDomain, SpaceCoord> {
public:
    typedef CartesianDomain DomainType;
    typedef SpaceCoord CoordType;
    typedef CartesianStagger::GridType GridType;
    typedef CartesianStagger::Location Location;
public:
    CartesianMesh(CartesianDomain &domain, uword haloWidth = 1)
            : StructuredMesh<CartesianDomain, SpaceCoord>(domain, haloWidth) {
    }
    virtual ~CartesianMesh() {}

    virtual void
    init(const string &filePath);

    virtual void
    init(uword nx, uword ny, uword nz = 1);

    virtual void
    setCellVolumes();

    int
    nx(int gridType, bool hasHaloGrids = false) const {
        return numGrid(0, gridType, hasHaloGrids);
    }

    int
    ny(int gridType, bool hasHaloGrids = false) const {
        return numGrid(1, gridType, hasHaloGrids);
    }

    int
    nz(int gridType, bool hasHaloGrids = false) const {
        return numGrid(2, gridType, hasHaloGrids);
    }

    void
    move(const SpaceCoord &x0, double dt, const Velocity &v,
         const CartesianMeshIndex &idx, SpaceCoord &x1) const;
}; // CartesianMesh

} // geomtk

#endif // __GEOMTK_CartesianMesh__
