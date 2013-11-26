#ifndef __SphereDomain__
#define __SphereDomain__

#include "Domain.h"

class SphereDomain : public Domain {
    double radius;
public:
    SphereDomain();
    SphereDomain(int numDim);
    virtual ~SphereDomain();

    void reverseLatitudeAxis();
    double getRadius();
    void setRadius(double radius);
};

#endif
