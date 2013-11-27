#ifndef __Field__
#define __Field__

#include "commons.h"
#include "Mesh.h"
#include "TimeLevels.h"

class Field {
protected:
    Mesh *mesh;
public:
    Field(Mesh &mesh);
    virtual ~Field();

    virtual void applyBndCond(int timeLevel) = 0;

    const Mesh& getMesh();
};

#endif
