#ifndef __StructuredScalarField__
#define __StructuredScalarField__

#include "Field.h"

class StructuredScalarField : public Field {
protected:
    TimeLevels<cube, 2> data;
    StaggerType *staggerTypes;
public:
    StructuredScalarField(Mesh &mesh);
    virtual ~StructuredScalarField();

    virtual void applyBndCond(int timeLevel);

    virtual void create(StaggerType xStaggerType,
                        StaggerType yStaggerType);
    virtual void create(StaggerType xStaggerType,
                        StaggerType yStaggerType,
                        StaggerType zStaggerType);

    /**
     * @brief Subscript operator of scalar field.
     */
    double operator()(int timeLevel, int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int i, int j, int k = 0);
};

#endif
