#ifndef __Geomtk_CartesianVelocityField__
#define __Geomtk_CartesianVelocityField__

#include "StructuredVelocityField.h"
#include "CartesianMesh.h"
#include "CartesianField.h"

namespace geomtk {

class CartesianVelocityField
: public StructuredVelocityField<CartesianMesh, CartesianField<double, 2> > {
public:
    CartesianVelocityField()
        : StructuredVelocityField<CartesianMesh, CartesianField<double, 2>>() {}
    virtual ~CartesianVelocityField() {}

    virtual void
    calcDivergence(const TimeLevelIndex<2> &timeIdx);

    virtual void
    calcVorticity(const TimeLevelIndex<2> &timeIdx);
};

} // geomtk

#endif // __Geomtk_CartesianVelocityField__