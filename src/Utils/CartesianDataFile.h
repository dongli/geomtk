#ifndef __Geomtk_CartesianDataFile__
#define __Geomtk_CartesianDataFile__

#include "StructuredDataFile.h"
#include "CartesianField.h"

namespace geomtk {

struct CartesianSpaceDimensions : public StructuredSpaceDimensions {
};

class CartesianDataFile : public StructuredDataFile<CartesianMesh> {
public:
	CartesianDataFile(MeshType &mesh) : StructuredDataFile<CartesianMesh>(mesh) {}
	virtual ~CartesianDataFile() {}
};

} // geomtk

#endif // __Geomtk_CartesianDataFile__
