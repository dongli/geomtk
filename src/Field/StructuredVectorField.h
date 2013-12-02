#ifndef __StructuredVectorField__
#define __StructuredVectorField__

#include "Field.h"
#include "StructuredMesh.h"

class StructuredVectorField : public Field {
protected:
    TimeLevels<cube, 2> data[3];
    StaggerType **staggerTypes;
public:
    StructuredVectorField(Mesh &mesh);
    virtual ~StructuredVectorField();

    virtual void applyBndCond(int timeLevel);

    virtual void create(StaggerType xLonStaggerType,
                        StaggerType xLatStaggerType,
                        StaggerType yLonStaggerType,
                        StaggerType yLatStaggerType);
    virtual void create(StaggerType xLonStaggerType,
                        StaggerType xLatStaggerType,
                        StaggerType xLevStaggerType,
                        StaggerType yLonStaggerType,
                        StaggerType yLatStaggerType,
                        StaggerType yLevStaggerType,
                        StaggerType zLonStaggerType,
                        StaggerType zLatStaggerType,
                        StaggerType zLevStaggerType);

    double operator()(int timeLevel, int dim, int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int dim, int i, int j, int k = 0);

    virtual StaggerType getStaggerType(int comp, int dim) const;
};

#endif
