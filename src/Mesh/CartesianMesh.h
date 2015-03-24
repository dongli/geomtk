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
    CartesianMesh(CartesianDomain &domain, int haloWidth = 1)
            : StructuredMesh<CartesianDomain, SpaceCoord>(domain, haloWidth) {
        this->_type = CARTESIAN_MESH;
    }
    virtual ~CartesianMesh() {}

    virtual void setCellVolumes();

    int nx(int gridType, bool hasHaloGrids = false) const { numGrid(0, gridType, hasHaloGrids); }

    int ny(int gridType, bool hasHaloGrids = false) const { numGrid(1, gridType, hasHaloGrids); }

    int nz(int gridType, bool hasHaloGrids = false) const { numGrid(2, gridType, hasHaloGrids); }

    void move(const SpaceCoord &x0, double dt, const Velocity &v,
              const CartesianMeshIndex &idx, SpaceCoord &x1) const;
};

} // geomtk

#endif // __Geomtk_CartesianMesh__
