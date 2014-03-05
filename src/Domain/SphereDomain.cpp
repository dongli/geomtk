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

void SphereCoord::setCoord(double lon, double lat) {
    coord[0] = lon;
    coord[1] = lat;
    updateTrigonometricFunctions();
}
    
void SphereCoord::setCoordComp(int dim, double comp) {
    coord(dim) = comp;
    updateTrigonometricFunctions();
}
    
void SphereCoord::setCartCoord(double x, double y, double z) {
    cartCoord[0] = x;
    cartCoord[1] = y;
    cartCoord[2] = z;
    coord[0] = atan2(y, x);
    coord[1] = asin(z);
    if (coord[0] < 0.0) coord[0] += PI2;
    if (coord[0] > PI2) coord[0] -= PI2;
    updateTrigonometricFunctions();
}

void SphereCoord::updateTrigonometricFunctions() {
    cosLon = cos(coord(0));
    sinLon = sin(coord(0));
    cosLat = cos(coord(1));
    sinLat = sin(coord(1));
}

SphereCoord& SphereCoord::operator=(const SphereCoord &other) {
    if (this != &other) {
        SpaceCoord::operator=(other);
        xt = other.xt;
        cartCoord = other.cartCoord;
        cosLon = other.cosLon;
        sinLon = other.sinLon;
        cosLat = other.cosLat;
        sinLat = other.sinLat;
    }
    return *this;
}

void SphereCoord::transformToPS(const SphereDomain &domain) {
    double tanLat = tan(coord(1));
    if (coord(1) < 0.0) { // South Pole
        xt(0) = -domain.getRadius()*sinLon/tanLat;
        xt(1) = -domain.getRadius()*cosLon/tanLat;
    } else { // North Pole
        xt(0) =  domain.getRadius()*cosLon/tanLat;
        xt(1) =  domain.getRadius()*sinLon/tanLat;
    }
    if (domain.getNumDim() == 3) {
        xt(2) = coord(2);
    }
}

void SphereCoord::transformFromPS(const SphereDomain &domain) {
    if (coord(1) < 0.0) { // South Pole
        coord(0) = atan2(xt(0), xt(1));
        coord(1) = -atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    } else { // North Pole
        coord(0) = atan2(xt(1), xt(0));
        coord(1) = atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    }
    if (coord(0) < 0.0) {
        coord(0) += PI2;
    }
    if (domain.getNumDim() == 3) {
        coord(2) = xt(2);
    }
    updateTrigonometricFunctions();
}

void SphereCoord::transformFromPS(const SphereDomain &domain, Pole pole) {
    if (pole == SOUTH_POLE) { // South Pole
        coord(0) =  atan2(xt(0), xt(1));
        coord(1) = -atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    } else { // North Pole
        coord(0) =  atan2(xt(1), xt(0));
        coord(1) =  atan(domain.getRadius()/sqrt(xt(0)*xt(0)+xt(1)*xt(1)));
    }
    if (coord(0) < 0.0) {
        coord(0) += PI2;
    }
    if (domain.getNumDim() == 3) {
        coord(2) = xt(2);
    }
    updateTrigonometricFunctions();
}

void SphereCoord::transformToCart(const SphereDomain &domain) {
    cartCoord(0) = domain.getRadius()*cosLat*cosLon;
    cartCoord(1) = domain.getRadius()*cosLat*sinLon;
    cartCoord(2) = domain.getRadius()*sinLat;
}

void SphereCoord::print() const {
    cout << "Coordinate:";
    cout << setw(20) << setprecision(10) << coord(0)/RAD;
    cout << setw(20) << setprecision(10) << coord(1)/RAD;
    if (coord.size() == 3) {
        cout << setw(20) << setprecision(10) << coord(2);
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
    transformToPS(x.getSinLat(), x.getSinLat()*x.getSinLat(),
                  x.getSinLon(), x.getCosLon());
}

void SphereVelocity::transformToPS(double sinLat, double sinLat2,
                                   double sinLon, double cosLon) {
    if (sinLat < 0.0) { // South Pole
        vt[0] = -cosLon/sinLat*v(0)+sinLon/sinLat2*v(1);
        vt[1] =  sinLon/sinLat*v(0)+cosLon/sinLat2*v(1);
    } else { // North Pole
        vt[0] = -sinLon/sinLat*v(0)-cosLon/sinLat2*v(1);
        vt[1] =  cosLon/sinLat*v(0)-sinLon/sinLat2*v(1);
    }
}

void SphereVelocity::transformFromPS(const SphereCoord &x) {
    if (x(1) < 0.0) { // South Pole
        v(0) = (x.getCosLon()*vt[0]+x.getSinLon()*vt[1])*x.getSinLat()*x.getSinLat();
        v(1) = (x.getSinLon()*vt[0]-x.getCosLon()*vt[1])*x.getSinLat();
    } else { // North Pole
        v(0) = (-x.getSinLon()*vt[0]+x.getCosLon()*vt[1])*x.getSinLat()*x.getSinLat();
        v(1) = (-x.getCosLon()*vt[0]-x.getSinLon()*vt[1])*x.getSinLat();
    }
}

void SphereVelocity::print() const {
    Velocity::print();
    cout << "Transformed velocity:";
    cout << setw(20) << setprecision(10) << vt[0];
    cout << setw(20) << setprecision(10) << vt[1] << endl;
}

// -----------------------------------------------------------------------------

SphereDomain::SphereDomain() {
    SphereDomain(2);
}

SphereDomain::SphereDomain(int numDim) : Domain(numDim) {
    radius = 1.0;
    setAxis(0, "lon", "longitude", "degree_east",
            0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, "lat", "latitude", "degree_north",
            -M_PI_2, POLE, M_PI_2, POLE);
    // TODO: Handle the vertical axis if has.
}

SphereDomain::~SphereDomain() {
}

double SphereDomain::calcDistance(const SphereCoord &x,
                                  const SphereCoord &y) const {
    double dlon = x(0)-y(0);
    double tmp1 = x.getSinLat()*y.getSinLat();
    double tmp2 = x.getCosLat()*y.getCosLat()*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}

double SphereDomain::calcDistance(const SphereCoord &x, double lon,
                                  double lat) const {
    double dlon = x(0)-lon;
    double tmp1 = x.getSinLat()*sin(lat);
    double tmp2 = x.getCosLat()*cos(lat)*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}

double SphereDomain::calcDistance(const SphereCoord &x, double lon,
                                  double sinLat, double cosLat) const {
    double dlon = x(0)-lon;
    double tmp1 = x.getSinLat()*sinLat;
    double tmp2 = x.getCosLat()*cosLat*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return radius*acos(tmp3);
}
    
vec SphereDomain::diffCoord(const SphereCoord &x,
                            const SphereCoord &y) const {
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

void SphereDomain::rotate(const SphereCoord &xp, const SphereCoord &xo,
                          SphereCoord &xr) const {
    double dlon = xo(0)-xp(0);
    double cosDlon = cos(dlon);
    double sinDlon = sin(dlon);

    double tmp1, tmp2, tmp3, lon, lat;

    tmp1 = xo.getCosLat()*sinDlon;
    tmp2 = xo.getCosLat()*xp.getSinLat()*cosDlon-xp.getCosLat()*xo.getSinLat();
    lon = atan2(tmp1, tmp2);
    if (lon < 0.0) lon += PI2;

    tmp1 = xo.getSinLat()*xp.getSinLat();
    tmp2 = xo.getCosLat()*xp.getCosLat()*cosDlon;
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
    lat = asin(tmp3);
    xr.setCoord(lon, lat);
}
    
void SphereDomain::rotateBack(const SphereCoord &xp, SphereCoord &xo,
                              const SphereCoord &xr) const {
    double tmp1, tmp2, tmp3, lon, lat;
    
    tmp1 = xr.getCosLat()*xr.getSinLon();
    tmp2 = xr.getSinLat()*xp.getCosLat()+xr.getCosLat()*xr.getCosLon()*xp.getSinLat();
#ifdef DEBUG
    static const double eps = 1.0e-15;
    if (fabs(tmp2) < eps) {
        //REPORT_WARNING("tmp2 is near zero!")
        tmp2 = 0.0;
    }
#endif
    lon = xp(0)+atan2(tmp1, tmp2);
    if (lon > PI2) lon -= PI2;
    if (lon < 0.0) lon += PI2;
    
    tmp1 = xr.getSinLat()*xp.getSinLat();
    tmp2 = xr.getCosLat()*xp.getCosLat()*xr.getCosLon();
    tmp3 = tmp1-tmp2;
#ifdef DEBUG
    if (tmp3 < -1.0 || tmp3 > 1.0)
        REPORT_ERROR("tmp3 is out of range [-1,1]!");
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    lat = asin(tmp3);
    xo.setCoord(lon, lat);
}

void SphereDomain::project(ProjectionType projType, const SphereCoord &xp,
                           const SphereCoord &xo, vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            SphereCoord xr(numDim);
            rotate(xp, xo, xr);
            xr.transformToPS(*this);
            xs(0) = xr[0];
            xs(1) = xr[1];
            break;
    }
}
 
void SphereDomain::projectBack(ProjectionType projType, const SphereCoord &xp,
                               SphereCoord &xo, const vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            SphereCoord xr(numDim);
            xr[0] = xs(0);
            xr[1] = xs(1);
            xr.transformFromPS(*this, NORTH_POLE);
            rotateBack(xp, xo, xr);
            break;
    }
}

string SphereDomain::getBrief() const {
    static string brief = "sphere domain";
    return brief;
}

}
