#ifndef __RLLRegrid__
#define __RLLRegrid__

#include "Regrid.h"

class RLLRegrid : public Regrid {
protected:
public:
    RLLRegrid(Mesh &mesh);
    virtual ~RLLRegrid();

    virtual void run(const Field &f, const SpaceCoord &x, double &y);
};

#endif
