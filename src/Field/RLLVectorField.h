#ifndef __RLLVectorField__
#define __RLLVectorField__

#include "Field.h"
#include "RLLMesh.h"

class RLLVectorField : public Field {
protected:
    TimeLevels<cube, 2> data[3];
    StaggerType **staggerTypes;
public:
    RLLVectorField(Mesh &mesh);
    virtual ~RLLVectorField();

    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType);
    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType uLevStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType,
                        StaggerType vLevStaggerType,
                        StaggerType wLonStaggerType,
                        StaggerType wLatStaggerType,
                        StaggerType wLevStaggerType);
};

#endif
