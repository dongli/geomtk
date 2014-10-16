#include "SphereVelocity.h"

namespace geomtk {

SphereVelocity::SphereVelocity() {
}

SphereVelocity::SphereVelocity(int numDim) : Velocity(numDim) {
}

SphereVelocity::~SphereVelocity() {
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
    // TODO: Clarify that we operate on vt directly is properly.
    res.vt[0] = vt[0]+other.vt[0];
    res.vt[1] = vt[1]+other.vt[1];
    return res;
}

const SphereVelocity SphereVelocity::operator-(const SphereVelocity &other) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator-(other);
    // TODO: Clarify that we operate on vt directly is properly.
    res.vt[0] = vt[0]-other.vt[0];
    res.vt[1] = vt[1]-other.vt[1];
    return res;
}

const SphereVelocity SphereVelocity::operator*(double scale) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator*(scale);
    // TODO: Clarify that we operate on vt directly is properly.
    res.vt[0] = vt[0]*scale;
    res.vt[1] = vt[1]*scale;
    return res;
}

const SphereVelocity SphereVelocity::operator/(double scale) const {
    SphereVelocity res;
    Velocity &tmp = res; tmp = Velocity::operator/(scale);
    // TODO: Clarify that we operate on vt directly is properly.
    res.vt[0] = vt[0]/scale;
    res.vt[1] = vt[1]/scale;
    return res;
}

void SphereVelocity::transformToPS(const SphereCoord &x) {
    transformToPS(x.sinLat(), x.sinLat()*x.sinLat(),
                  x.sinLon(), x.cosLon());
}

void SphereVelocity::transformToPS(double sinLat, double sinLat2,
                                   double sinLon, double cosLon) {
    if (sinLat < 0.0) { // South Pole
        vt[0] = -sinLon/sinLat*v[0]-cosLon/sinLat2*v[1];
        vt[1] = -cosLon/sinLat*v[0]+sinLon/sinLat2*v[1];
    } else { // North Pole
        vt[0] = -sinLon/sinLat*v[0]-cosLon/sinLat2*v[1];
        vt[1] =  cosLon/sinLat*v[0]-sinLon/sinLat2*v[1];
    }
}

void SphereVelocity::transformFromPS(const SphereCoord &x) {
    if (x(1) < 0.0) { // South Pole
        v[0] = (-x.sinLon()*vt[0]-x.cosLon()*vt[1])*x.sinLat();
        v[1] = (-x.cosLon()*vt[0]+x.sinLon()*vt[1])*x.sinLat()*x.sinLat();
    } else { // North Pole
        v[0] = (-x.sinLon()*vt[0]+x.cosLon()*vt[1])*x.sinLat();
        v[1] = (-x.cosLon()*vt[0]-x.sinLon()*vt[1])*x.sinLat()*x.sinLat();
    }
}

void SphereVelocity::print() const {
    Velocity::print();
    cout << "Transformed velocity:";
    cout << setw(20) << setprecision(10) << vt[0];
    cout << setw(20) << setprecision(10) << vt[1] << endl;
}

} // geomtk
