#ifndef __Regrid__
#define __Regrid__

#include "commons.h"
#include "Mesh.h"
#include "Field.h"

/**
 * @class Regrid
 * @brief This is the base class that provides regridding operations. Concrete
 * regrid child classes will be bounded to a specific mesh. 
 */

class Regrid {
protected:
    Mesh *mesh;
public:
    Regrid(Mesh &mesh);
    virtual ~Regrid();

    virtual void run(const Field &f, const SpaceCoord &x, double &y) = 0;
};

#endif
