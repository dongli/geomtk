#include "SphereDomain.h"

SphereDomain::SphereDomain() : Domain(2) {
    setAxis(0, 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, -M_PI_2, POLE, M_PI_2, POLE);
}

SphereDomain::SphereDomain(int numDim) : Domain(numDim) {
    setAxis(0, 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, -M_PI_2, POLE, M_PI_2, POLE);
}

SphereDomain::~SphereDomain() {
}

void SphereDomain::reverseLatitudeAxis() {
    setAxis(1, M_PI_2, POLE, -M_PI_2, POLE);
}

inline double SphereDomain::getRadius() {
    return radius;
}

void SphereDomain::setRadius(double radius) {
    this->radius = radius;
}
