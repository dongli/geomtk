#ifndef __GEOMTK_CartesianField__
#define __GEOMTK_CartesianField__

#include "StructuredField.h"
#include "CartesianMesh.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on Cartesian mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <typename DataType, int NumTimeLevel = 1>
class CartesianField : public StructuredField<CartesianMesh, DataType, NumTimeLevel> {
public:
	typedef CartesianMesh MeshType;

    CartesianField() {}
    virtual ~CartesianField() {}
}; // CartesianField

} // geomtk

#endif // __GEOMTK_CartesianField__
