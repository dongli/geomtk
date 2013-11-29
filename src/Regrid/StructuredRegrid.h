#ifndef __StructuredRegrid__
#define __StructuredRegrid__

#include "Regrid.h"

class StructuredRegrid : public Regrid {
protected:
public:
    StructuredRegrid(Mesh &mesh);
    virtual ~StructuredRegrid();

    virtual void run(const Field &f, const SpaceCoord &x, double &y);
};

#endif
