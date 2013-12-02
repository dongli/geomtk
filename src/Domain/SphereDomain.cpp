#include "SphereDomain.h"

SphereCoord::SphereCoord(int numDim) : SpaceCoord(numDim) {
}

SphereCoord::~SphereCoord() {
}

// -----------------------------------------------------------------------------

SphereVelocity::SphereVelocity() {
}

SphereVelocity::SphereVelocity(int numDim) : Velocity(numDim) {
}

SphereVelocity::~SphereVelocity() {
}

double SphereVelocity::operator[](int dim) const {
    if (dim > 2) {
        REPORT_ERROR("Transformed velocity is only 2D!");
    }
    return vt[dim];
}

double& SphereVelocity::operator[](int dim) {
    if (dim > 2) {
        REPORT_ERROR("Transformed velocity is only 2D!");
    }
    return vt[dim];
}

void SphereVelocity::transformToPS(const SpaceCoord &x) {
    double sign = x(1) < 0.0 ? -1.0 : 1.0;
    double sinLat = sin(x(1));
    double sinLat2 = sinLat*sinLat;
    double sinLon = sin(x(0));
    double cosLon = cos(x(0));
    vt[0] = sign*(-sinLon/sinLat*v(0)-cosLon/sinLat2*v(1));
    vt[1] = sign*( cosLon/sinLat*v(0)-sinLon/sinLat2*v(1));
}

void SphereVelocity::transformToPS(double sinLat, double sinLat2,
                                   double sinLon, double cosLon) {
    double sign = sinLat < 0.0 ? -1.0 : 1.0;
    vt[0] = sign*(-sinLon/sinLat*v(0)-cosLon/sinLat2*v(1));
    vt[1] = sign*( cosLon/sinLat*v(0)-sinLon/sinLat2*v(1));
}

void SphereVelocity::print() const {
    cout << "Transformed velocity: ";
    cout << setw(20) << vt[0] << setw(20) << vt[1] << endl;
}

// -----------------------------------------------------------------------------

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

inline double SphereDomain::getRadius() {
    return radius;
}

void SphereDomain::setRadius(double radius) {
    this->radius = radius;
}

double SphereDomain::calcDistance(const SpaceCoord &x,
                                  const SpaceCoord &y) const {
    double dlon = x(0)-y(0);
    double tmp1 = sin(x(1))*sin(y(1));
    double tmp2 = cos(x(1))*cos(y(1))*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}

double SphereDomain::calcDistance(const SpaceCoord &x, double lon,
                                  double lat) const {
    double dlon = x(0)-lon;
    double tmp1 = sin(x(1))*sin(lat);
    double tmp2 = cos(x(1))*cos(lat)*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}

double SphereDomain::calcDistance(const SpaceCoord &x, double lon,
                                  double sinLat, double cosLat) const {
    double dlon = x(0)-lon;
    double tmp1 = sin(x(1))*sinLat;
    double tmp2 = cos(x(1))*cosLat*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}
