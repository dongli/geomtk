#include "SphereDomain.h"

SphereDomain::SphereDomain(int numDim) : Domain(numDim) {
    setAxis(0, 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(0, -M_PI_2, POLE, M_PI_2, POLE);
}

inline double SphereDomain::getRadius() {
    return radius;
}

void SphereDomain::setRadius(double radius) {
    this->radius = radius;
}
