#ifndef __RLLScalarField__
#define __RLLScalarField__

#include "Field.h"
#include "RLLMesh.h"

class RLLScalarField : public Field {
protected:
    TimeLevels<cube, 2> data;
    StaggerType *staggerTypes;
public:
    RLLScalarField(Mesh &mesh);
    virtual ~RLLScalarField();

    virtual void applyBndCond(int timeLevel);

    virtual void create(StaggerType lonStaggerType,
                        StaggerType latStaggerType);
    virtual void create(StaggerType lonStaggerType,
                        StaggerType latStaggerType,
                        StaggerType levStaggerType);

    double operator()(int timeLevel, int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int i, int j, int k = 0);
};

#endif

