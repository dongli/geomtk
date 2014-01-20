#include "SphereDomain.h"

namespace geomtk {

SphereCoord::SphereCoord(int numDim) : SpaceCoord(numDim) {
    xt.set_size(numDim);
}

SphereCoord::SphereCoord(const SphereCoord &other) : SpaceCoord(other) {
    *this = other;
}

SphereCoord::~SphereCoord() {
}

double SphereCoord::operator[](int i) const {
    return xt(i);
}

double& SphereCoord::operator[](int i) {
    return xt(i);
}

const vec& SphereCoord::getPSCoord() const {
    return xt;
}

vec& SphereCoord::getPSCoord() {
    return xt;
}

SphereCoord& SphereCoord::operator=(const SphereCoord &other) {
    if (this != &other) {
        SpaceCoord::operator=(other);
        xt = other.xt;
        cartCoord = other.cartCoord;
    }
    return *this;
}

void SphereCoord::transformToPS(const SphereDomain &domain) {
    double sign = coord(1) < 0.0 ? -1.0 : 1.0;
    double tanLat = tan(coord(1));
    xt(0) = sign*domain.getRadius()*cos(coord(0))/tanLat;
    xt(1) = sign*domain.getRadius()*sin(coord(0))/tanLat;
    if (domain.getNumDim() == 3) {
        xt(2) = coord(2);
    }
}

void SphereCoord::transformFromPS(const SphereDomain &domain) {
    double sign = coord(1) < 0.0 ? -1.0 : 1.0;
    coord(0) = atan2(xt(1), xt(0));
    if (coord(0) < 0.0) {
        coord(0) += PI2;
    }
    coord(1) = sign*atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    if (domain.getNumDim() == 3) {
        coord(2) = xt(2);
    }
}

void SphereCoord::transformFromPS(const SphereDomain &domain, Pole pole) {
    double sign = pole == SOUTH_POLE ? -1.0 : 1.0;
    coord(0) = atan2(xt(1), xt(0));
    if (coord(0) < 0.0) {
        coord(0) += PI2;
    }
    coord(1) = sign*atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    if (domain.getNumDim() == 3) {
        coord(2) = xt(2);
    }
}

void SphereCoord::transformToCart(const SphereDomain &domain) {
    double cosLat = cos(coord(1));
    cartCoord(0) = domain.getRadius()*cosLat*cos(coord(0));
    cartCoord(1) = domain.getRadius()*cosLat*sin(coord(0));
    cartCoord(2) = domain.getRadius()*sin(coord(1));
}
    
const vec& SphereCoord::getCartCoord() const {
    return cartCoord;
}

void SphereCoord::print() const {
    cout << "Coordinate:";
    cout << setw(20) << coord(0)/RAD;
    cout << setw(20) << coord(1)/RAD;
    if (coord.size() == 3) {
        cout << setw(20) << coord(2);
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
    
vec SphereDomain::diffCoord(const SpaceCoord &x,
                            const SpaceCoord &y) const {
    vec d(numDim);
    for (int m = 0; m < numDim; ++m) {
        d(m) = x(m)-y(m);
    }
    if (fabs(d(0)) > M_PI_2) {
        if (d(0) > 0.0) {
            d(0) -= PI2;
        } else {
            d(0) += PI2;
        }
    }
    return d;
}

void SphereDomain::rotate(const SpaceCoord &xp, const SpaceCoord &xo,
                          SpaceCoord &xr) const {
    double dlon = xo(0)-xp(0);
    double cosLatP = cos(xp(1));
    double sinLatP = sin(xp(1));
    double cosLatO = cos(xo(1));
    double sinLatO = sin(xo(1));
    double cosDlon = cos(dlon);
    double sinDlon = sin(dlon);

    double tmp1, tmp2, tmp3;

    tmp1 = cosLatO*sinDlon;
    tmp2 = cosLatO*sinLatP*cosDlon-cosLatP*sinLatO;
    xr(0) = atan2(tmp1, tmp2);
    if (xr(0) < 0.0) xr(0) += PI2;

    tmp1 = sinLatO*sinLatP;
    tmp2 = cosLatO*cosLatP*cosDlon;
    tmp3 = tmp1+tmp2;
#ifdef DEBUG
    static const double eps = 1.0e-15;
    if (tmp3 < -1.0 || tmp3 > 1.0) {
        if (fabs(tmp3)-1.0 < eps) {
            REPORT_WARNING("tmp3 is out of range [-1, 1]!");
        } else
            REPORT_ERROR("tmp3 is out of range [-1, 1]!");
    }
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    xr(1) = asin(tmp3);
}
    
void SphereDomain::rotateBack(const SpaceCoord &xp, SpaceCoord &xo,
                              const SpaceCoord &xr) const {
    double cosLatP = cos(xp(1));
    double sinLatP = sin(xp(1));
    double cosLonR = cos(xr(0));
    double sinLonR = sin(xr(0));
    double cosLatR = cos(xr(1));
    double sinLatR = sin(xr(1));
    
    double tmp1, tmp2, tmp3;
    
    tmp1 = cosLatR*sinLonR;
    tmp2 = sinLatR*cosLatP+cosLatR*cosLonR*sinLatP;
#ifdef DEBUG
    static const double eps = 1.0e-15;
    if (fabs(tmp2) < eps) {
        //REPORT_WARNING("tmp2 is near zero!")
        tmp2 = 0.0;
    }
#endif
    xo(0) = xp(0)+atan2(tmp1, tmp2);
    if (xo(0) > PI2) xo(0) -= PI2;
    if (xo(0) < 0.0) xo(0) += PI2;
    
    tmp1 = sinLatR*sinLatP;
    tmp2 = cosLatR*cosLatP*cosLonR;
    tmp3 = tmp1-tmp2;
#ifdef DEBUG
    if (tmp3 < -1.0 || tmp3 > 1.0)
        REPORT_ERROR("tmp3 is out of range [-1,1]!");
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    xo(1) = asin(tmp3);
}

string SphereDomain::getBrief() const {
    string brief = "sphere domain";
    return brief;
}

}
