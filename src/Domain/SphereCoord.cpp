#include "SphereCoord.h"
#include "SphereDomain.h"

namespace geomtk {

SphereCoord::SphereCoord() : SpaceCoord() {
}

SphereCoord::SphereCoord(int numDim) : SpaceCoord(numDim) {
    xt.set_size(numDim);
}

SphereCoord::SphereCoord(const SphereCoord &other) : SpaceCoord(other) {
    *this = other;
}

SphereCoord::~SphereCoord() {
}

void SphereCoord::setNumDim(int numDim) {
    xt.set_size(numDim);
    SpaceCoord::setNumDim(numDim);
}

void SphereCoord::setCoord(double lon, double lat) {
    coord[0] = lon;
    coord[1] = lat;
    updateTrigonometricFunctions();
}

void SphereCoord::setCoord(double lon, double lat, double lev) {
    coord[0] = lon;
    coord[1] = lat;
    coord[2] = lev;
    updateTrigonometricFunctions();
}
    
void SphereCoord::setCoordComp(int dim, double comp) {
    coord(dim) = comp;
    if (dim == 0) {
        _cosLon = cos(comp);
        _sinLon = sin(comp);
    } else if (dim == 1) {
        _cosLat = cos(comp);
        _sinLat = sin(comp);
    }
}
    
void SphereCoord::setCartCoord(double x, double y, double z) {
    _cartCoord[0] = x;
    _cartCoord[1] = y;
    _cartCoord[2] = z;
    coord[0] = atan2(y, x);
    coord[1] = asin(z);
    if (coord[0] < 0.0) coord[0] += PI2;
    if (coord[0] > PI2) coord[0] -= PI2;
    updateTrigonometricFunctions();
}

void SphereCoord::updateTrigonometricFunctions() {
    _cosLon = cos(coord[0]);
    _sinLon = sin(coord[0]);
    _cosLat = cos(coord[1]);
    _sinLat = sin(coord[1]);
}

SphereCoord& SphereCoord::operator=(const SphereCoord &other) {
    if (this != &other) {
        SpaceCoord::operator=(other);
        xt = other.xt;
        _cartCoord = other.cartCoord();
        _cosLon = other.cosLon();
        _sinLon = other.sinLon();
        _cosLat = other.cosLat();
        _sinLat = other.sinLat();
    }
    return *this;
}

void SphereCoord::transformToPS(const SphereDomain &domain) {
    double tanLat = tan(coord[1]);
    if (coord[1] < 0.0) { // South Pole
        xt[0] =  domain.radius()*_cosLon/tanLat;
        xt[1] = -domain.radius()*_sinLon/tanLat;
    } else { // North Pole
        xt[0] =  domain.radius()*_cosLon/tanLat;
        xt[1] =  domain.radius()*_sinLon/tanLat;
    }
    if (domain.numDim() == 3) {
        xt[2] = coord[2];
    }
}

void SphereCoord::transformFromPS(const SphereDomain &domain, Pole pole) {
    if (pole == SOUTH_POLE) { // South Pole
        coord[0] = atan2(xt[1], -xt[0]);
        coord[1] = -atan(domain.radius()/sqrt(xt[0]*xt[0]+xt[1]*xt[1]));
#ifndef NDEBUG
        assert(coord[1] < 0);
#endif
    } else { // North Pole
        coord[0] = atan2(xt[1], xt[0]);
        coord[1] = atan(domain.radius()/sqrt(xt[0]*xt[0]+xt[1]*xt[1]));
#ifndef NDEBUG
        assert(coord[1] > 0);
#endif
    }
    if (coord[0] < 0.0) coord[0] += PI2;
    if (coord[0] > PI2) coord[0] -= PI2;
    if (domain.numDim() == 3) {
        coord[2] = xt[2];
    }
    updateTrigonometricFunctions();
}

void SphereCoord::
transformToCart(const SphereDomain &domain) {
    _cartCoord[0] = domain.radius()*_cosLat*_cosLon;
    _cartCoord[1] = domain.radius()*_cosLat*_sinLon;
    _cartCoord[2] = domain.radius()*_sinLat;
}

void SphereCoord::print() const {
    cout << "Coordinate:";
    cout << setw(20) << setprecision(10) << coord[0]/RAD;
    cout << setw(20) << setprecision(10) << coord[1]/RAD;
    if (coord.size() == 3) {
        cout << setw(20) << setprecision(10) << coord(2);
    }
    cout << endl;
}

} // geomtk