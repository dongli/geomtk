#ifndef __GEOMTK_CartesianDataFile__
#define __GEOMTK_CartesianDataFile__

#include "StructuredDataFile.h"
#include "CartesianField.h"

namespace geomtk {

struct CartesianSpaceDimensions : public StructuredSpaceDimensions {
};

class CartesianDataFile : public StructuredDataFile<CartesianMesh> {
public:
    typedef CartesianSpaceDimensions SpaceDimensions;

	CartesianDataFile(MeshType &mesh, TimeManager &timeManager)
        : StructuredDataFile<CartesianMesh>(mesh, timeManager) {}
	virtual ~CartesianDataFile() {}

    virtual void
    inputHorizontalMesh();

    virtual void
    inputVerticalMesh();

    virtual void
    outputDomain();
};

} // geomtk

#endif // __GEOMTK_CartesianDataFile__
