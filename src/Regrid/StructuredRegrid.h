#ifndef __StructuredRegrid__
#define __StructuredRegrid__

#include "Regrid.h"

namespace geomtk {

class StructuredRegrid : public Regrid {
protected:
public:
    StructuredRegrid(Mesh &mesh);
    virtual ~StructuredRegrid();

    virtual void run(RegridMethod method, int timeLevel, const Field &f,
                     const SpaceCoord &x, double &y, MeshIndex *idx = NULL);
    virtual void run(RegridMethod method, int timeLevel, const Field &f,
                     const SpaceCoord &x, vec &y, MeshIndex *idx = NULL);
};

}

#endif
