#ifndef __Geomtk_StructuredTensorField__
#define __Geomtk_StructuredTensorField__

#include "StructuredVectorField.h"

namespace geomtk {

/**
 *  StructuredTensorField
 *
 *  This class describes the tensor field, which can be calculated from a
 *  vector field. The tensor field is assumed on A grid.
 */

class StructuredTensorField : public Field {
protected:
    TimeLevels<cube, 2> ***data;
public:
    StructuredTensorField(const Mesh &mesh, bool hasHalfLevel = false);
    StructuredTensorField(const string &name, const string &units,
                          const string &longName, const Mesh &mesh,
                          bool hasHalfLevel = false);
    virtual ~StructuredTensorField();

    virtual void applyBndCond(int timeLevel, bool updateHalfLevel = false);

    virtual void create();

    void calcFromVector(const StructuredVectorField &vector, int timeLevel);

    double operator()(int timeLevel, int dim1, int dim2,
                      int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int dim1, int dim2,
                       int i, int j, int k = 0);
};

}

#endif
