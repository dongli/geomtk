#ifndef __GEOMTK_CartesianMeshIndex__
#define __GEOMTK_CartesianMeshIndex__

#include "StructuredMeshIndex.h"
#include "CartesianMesh.h"

namespace geomtk {

class CartesianMeshIndex : public StructuredMeshIndex<CartesianMesh, SpaceCoord> {
public:
    CartesianMeshIndex() : StructuredMeshIndex<CartesianMesh, SpaceCoord>() {}
	CartesianMeshIndex(int numDim) : StructuredMeshIndex<CartesianMesh, SpaceCoord>(numDim) {}
	virtual ~CartesianMeshIndex() {}
};

} // geomtk

#endif // __GEOMTK_CartesianMeshIndex__
