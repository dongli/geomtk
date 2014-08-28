#ifndef __Geomtk_CartesianField__
#define __Geomtk_CartesianField__

#include "StructuredField.h"
#include "CartesianMesh.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on Cartesian mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <typename DataType, int NumTimeLevel>
class CartesianField : public StructuredField<CartesianMesh, DataType, NumTimeLevel> {
public:
	typedef CartesianMesh MeshType;

    CartesianField() {}
    virtual ~CartesianField() {}
};

} // geomtk

#endif // __Geomtk_CartesianField__
