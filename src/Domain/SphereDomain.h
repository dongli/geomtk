#ifndef __SphereDomain__
#define __SphereDomain__

#include "Domain.h"

class SphereDomain : public Domain {
    double radius;
public:
    SphereDomain(int numDim);
    virtual ~SphereDomain();

    double getRadius();
    void setRadius(double radius);
};

#endif
