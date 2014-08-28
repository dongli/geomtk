#ifndef __Geomtk_CartesianMeshIndex__
#define __Geomtk_CartesianMeshIndex__

#include "StructuredMeshIndex.h"
#include "CartesianMesh.h"

namespace geomtk {

class CartesianMeshIndex : public StructuredMeshIndex<CartesianMesh, SpaceCoord> {
public:
	CartesianMeshIndex(int numDim) : StructuredMeshIndex<CartesianMesh, SpaceCoord>(numDim) {}
	virtual ~CartesianMeshIndex() {}
};

} // geomtk

#endif // __Geomtk_CartesianMeshIndex__
