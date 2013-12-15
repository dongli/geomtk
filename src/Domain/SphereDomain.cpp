#include "SphereDomain.h"

namespace geomtk {

SphereCoord::SphereCoord(int numDim) : SpaceCoord(numDim) {
}

SphereCoord::~SphereCoord() {
}

double SphereCoord::operator[](int i) const {
    return xt[i];
}

double& SphereCoord::operator[](int i) {
    return xt[i];
}

SphereCoord& SphereCoord::operator=(const SphereCoord &other) {
    if (this != &other) {
        SpaceCoord::operator=(other);
        this->xt[0] = other.xt[0];
        this->xt[1] = other.xt[1];
    }
    return *this;
}

void SphereCoord::transformToPS(const SphereDomain &domain) {
    double sign = coords(1) < 0.0 ? -1.0 : 1.0;
    double tanLat = tan(coords(1));
    xt[0] = sign*domain.getRadius()*cos(coords(0))/tanLat;
    xt[1] = sign*domain.getRadius()*sin(coords(0))/tanLat;
}

void SphereCoord::transformFromPS(const SphereDomain &domain) {
    double sign = coords(1) < 0.0 ? -1.0 : 1.0;
    coords(0) = atan2(xt[1], xt[0]);
    if (coords(0) < 0.0) {
        coords(0) += PI2;
    }
    coords(1) = sign*atan(domain.getRadius()/sqrt(xt[0]*xt[0]+xt[1]*xt[1]));
}

void SphereCoord::print() const {
    cout << "Coordinate:";
    cout << setw(20) << coords(0)/RAD;
    cout << setw(20) << coords(1)/RAD;
    if (coords.size() == 3) {
        cout << setw(20) << coords(2);
    }
    cout << endl;
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

SphereVelocity& SphereVelocity::operator=(const SphereVelocity &other) {
    if (this != &other) {
        Velocity::operator=(other);
        this->vt[0] = other.vt[0];
        this->vt[1] = other.vt[1];
    }
    return *this;
}

const SphereVelocity SphereVelocity::operator+(const SphereVelocity &other) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator+(other);
    // TODO: Clarify that we operator on vt directly is properly.
    res.vt[0] = vt[0]+other.vt[0];
    res.vt[1] = vt[1]+other.vt[1];
    return res;
}

const SphereVelocity SphereVelocity::operator-(const SphereVelocity &other) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator-(other);
    // TODO: Clarify that we operator on vt directly is properly.
    res.vt[0] = vt[0]-other.vt[0];
    res.vt[1] = vt[1]-other.vt[1];
    return res;
}

const SphereVelocity SphereVelocity::operator*(double scale) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator*(scale);
    // TODO: Clarify that we operator on vt directly is properly.
    res.vt[0] = vt[0]*scale;
    res.vt[1] = vt[1]*scale;
    return res;
}

const SphereVelocity SphereVelocity::operator/(double scale) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator/(scale);
    // TODO: Clarify that we operator on vt directly is properly.
    res.vt[0] = vt[0]/scale;
    res.vt[1] = vt[1]/scale;
    return res;
}

void SphereVelocity::transformToPS(const SpaceCoord &x) {
    double sinLat = sin(x(1));
    double sinLat2 = sinLat*sinLat;
    double sinLon = sin(x(0));
    double cosLon = cos(x(0));
    transformToPS(sinLat, sinLat2, sinLon, cosLon);
}

void SphereVelocity::transformToPS(double sinLat, double sinLat2,
                                   double sinLon, double cosLon) {
    double sign = sinLat < 0.0 ? -1.0 : 1.0;
    vt[0] = sign*(-sinLon/sinLat*v(0)-cosLon/sinLat2*v(1));
    vt[1] = sign*( cosLon/sinLat*v(0)-sinLon/sinLat2*v(1));
}

void SphereVelocity::transformFromPS(const SpaceCoord &x) {
    double sign = x(1) < 0.0 ? -1.0 : 1.0;
    double sinLat = sin(x(1));
    double sinLat2 = sinLat*sinLat;
    double sinLon = sin(x(0));
    double cosLon = cos(x(0));
    v(0) = sign*(-sinLon*vt[0]+cosLon*vt[1])*sinLat;
    v(1) = sign*(-cosLon*vt[0]-sinLon*vt[1])*sinLat2;
}

void SphereVelocity::print() const {
    Velocity::print();
    cout << "Transformed velocity:";
    cout << setw(20) << vt[0] << setw(20) << vt[1] << endl;
}

// -----------------------------------------------------------------------------

SphereDomain::SphereDomain() {
    SphereDomain(2);
}

SphereDomain::SphereDomain(int numDim) : Domain(numDim) {
    radius = 1.0;
    setAxis(0, 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, -M_PI_2, POLE, M_PI_2, POLE);
}

SphereDomain::~SphereDomain() {
}

double SphereDomain::getRadius() const {
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

string SphereDomain::getBrief() const {
    string brief = "sphere domain";
    return brief;
}

}
