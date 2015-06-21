#include "SphereDomain.h"

namespace geomtk {

SphereDomain::
SphereDomain() {
    SphereDomain(2);
}

SphereDomain::
SphereDomain(int numDim)
: Domain(numDim) {
    if (numDim != 2) {
        REPORT_ERROR("Spherical domain dimension should be 2 in this case!");
    }
    _radius = 1.0;
    setAxis(0, "lon", "longitude", "radian_east", 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, "lat", "latitude", "radian_north", -M_PI_2, POLE, M_PI_2, POLE);
}

SphereDomain::
SphereDomain(VertCoordType vertCoordType)
: Domain(vertCoordType) {
    _radius = 1.0;
    setAxis(0, "lon", "longitude", "radian_east", 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, "lat", "latitude", "radian_north", -M_PI_2, POLE, M_PI_2, POLE);
    switch (vertCoordType) {
        case CLASSIC_PRESSURE_SIGMA:
            setAxis(2, "lev", "classic pressure sigma level", "1", 0.0, RIGID, 1.0, RIGID);
            break;
        case HYBRID_PRESSURE_SIGMA:
            setAxis(2, "lev", "hybrid pressure sigma level", "1", 0.0, RIGID, 1.0, RIGID);
            break;
        case PRESSURE:
            _axisName[2] = "lev";
            _bndTypeStarts[2] = RIGID;
            _bndTypeEnds[2] = RIGID;
            // User should set the vertical pressure levels manually.
            break;
        default:
            REPORT_ERROR("Under construction!");
            break;
    }
}

SphereDomain::
~SphereDomain() {
}

void SphereDomain::
init(const string &filePath) {
    REPORT_ERROR("Under construction!");
} // init

double SphereDomain::
calcDistance(const SphereCoord &x, const SphereCoord &y) const {
    // TODO: We don't consider vertical distance yet!
    double dlon = x(0)-y(0);
    double tmp1 = x.sinLat()*y.sinLat();
    double tmp2 = x.cosLat()*y.cosLat()*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return _radius*acos(tmp3);
} // calcDistance

double SphereDomain::
calcDistance(const vec &x, const vec &y) const {
    return norm(x-y);
} // calcDistance

double SphereDomain::
calcDistance(const SphereCoord &x, double lon, double lat) const {
    double dlon = x(0)-lon;
    double tmp1 = x.sinLat()*sin(lat);
    double tmp2 = x.cosLat()*cos(lat)*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return _radius*acos(tmp3);
} // calcDistance

double SphereDomain::
calcDistance(const SphereCoord &x, double lon, double sinLat, double cosLat) const {
    double dlon = x(0)-lon;
    double tmp1 = x.sinLat()*sinLat;
    double tmp2 = x.cosLat()*cosLat*cos(dlon);
    double tmp3 = min(1.0, max(-1.0, tmp1+tmp2));
    return _radius*acos(tmp3);
} // calcDistance

vec SphereDomain::
diffCoord(const SphereCoord &x, const SphereCoord &y) const {
    vec d(numDim());
    for (uword m = 0; m < numDim(); ++m) {
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
} // diffCoord

vec SphereDomain::
diffCoord(const vec &x, const vec &y) const {
    vec d(numDim());
    for (uword m = 0; m < numDim(); ++m) {
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
} // diffCoord

void SphereDomain::
rotate(const SphereCoord &xp, const SphereCoord &xo, SphereCoord &xr) const {
    double dlon = xo(0)-xp(0);
    double cosDlon = cos(dlon);
    double sinDlon = sin(dlon);

    double tmp1, tmp2, tmp3, lon, lat;

    tmp1 = xo.cosLat()*sinDlon;
    tmp2 = xo.cosLat()*xp.sinLat()*cosDlon-xp.cosLat()*xo.sinLat();
    lon = atan2(tmp1, tmp2);
    if (lon < 0.0) lon += PI2;

    tmp1 = xo.sinLat()*xp.sinLat();
    tmp2 = xo.cosLat()*xp.cosLat()*cosDlon;
    tmp3 = tmp1+tmp2;
#ifndef NDEBUG
    static const double eps = 1.0e-15;
    if (tmp3 < -1.0 || tmp3 > 1.0) {
        if (fabs(tmp3)-1.0 > eps) {
            REPORT_ERROR("tmp3 (" << tmp3 << ") is out of range [-1, 1]!");
        }
    }
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    lat = asin(tmp3);
    xr.set(lon, lat);
} // rotate

void SphereDomain::
rotate(const SphereCoord &xp, const SphereCoord &xo, double &lonR, double &latR) const {
    double dlon = xo(0)-xp(0);
    double cosDlon = cos(dlon);
    double sinDlon = sin(dlon);
    
    double tmp1, tmp2, tmp3;
    
    tmp1 = xo.cosLat()*sinDlon;
    tmp2 = xo.cosLat()*xp.sinLat()*cosDlon-xp.cosLat()*xo.sinLat();
    lonR = atan2(tmp1, tmp2);
    if (lonR < 0.0) lonR += PI2;
    
    tmp1 = xo.sinLat()*xp.sinLat();
    tmp2 = xo.cosLat()*xp.cosLat()*cosDlon;
    tmp3 = tmp1+tmp2;
#ifndef NDEBUG
    static const double eps = 1.0e-15;
    if (tmp3 < -1.0 || tmp3 > 1.0) {
        if (fabs(tmp3)-1.0 > eps) {
            REPORT_ERROR("tmp3 (" << tmp3 << ") is out of range [-1, 1]!");
        }
    }
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    latR = asin(tmp3);
} // rotate

void SphereDomain::
rotateBack(const SphereCoord &xp, SphereCoord &xo, const SphereCoord &xr) const {
    double tmp1, tmp2, tmp3, lon, lat;
    
    tmp1 = xr.cosLat()*xr.sinLon();
    tmp2 = xr.sinLat()*xp.cosLat()+xr.cosLat()*xr.cosLon()*xp.sinLat();
#ifndef NDEBUG
    static const double eps = 1.0e-15;
    if (fabs(tmp2) < eps) {
//        REPORT_WARNING("tmp2 is near zero!");
        tmp2 = 0.0;
    }
#endif
    lon = xp(0)+atan2(tmp1, tmp2);
    if (lon > PI2) lon -= PI2;
    if (lon < 0.0) lon += PI2;
    
    tmp1 = xr.sinLat()*xp.sinLat();
    tmp2 = xr.cosLat()*xp.cosLat()*xr.cosLon();
    tmp3 = tmp1-tmp2;
#ifndef NDEBUG
    if (tmp3 < -1.0 || tmp3 > 1.0) {
        if (fabs(tmp3)-1.0 > eps) {
            REPORT_ERROR("tmp3 (" << tmp3 << ") is out of range [-1, 1]!");
        }
    }
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    lat = asin(tmp3);
    xo.set(lon, lat);
} // rotate
    
void SphereDomain::
rotateBack(const SphereCoord &xp, SphereCoord &xo, double lonR, double latR) const {
    double tmp1, tmp2, tmp3, lon, lat;
    
    double sinLonR = sin(lonR);
    double cosLonR = cos(lonR);
    double sinLatR = sin(latR);
    double cosLatR = cos(latR);
    
    tmp1 = cosLatR*sinLonR;
    tmp2 = sinLatR*xp.cosLat()+cosLatR*cosLonR*xp.sinLat();
#ifndef NDEBUG
    static const double eps = 1.0e-15;
    if (fabs(tmp2) < eps) {
//        REPORT_WARNING("tmp2 is near zero!");
        tmp2 = 0.0;
    }
#endif
    lon = xp(0)+atan2(tmp1, tmp2);
    if (lon > PI2) lon -= PI2;
    if (lon < 0.0) lon += PI2;
    
    tmp1 = sinLatR*xp.sinLat();
    tmp2 = cosLatR*xp.cosLat()*cosLonR;
    tmp3 = tmp1-tmp2;
#ifndef NDEBUG
    if (tmp3 < -1.0 || tmp3 > 1.0) {
        if (fabs(tmp3)-1.0 > eps) {
            REPORT_ERROR("tmp3 (" << tmp3 << ") is out of range [-1, 1]!");
        }
    }
#endif
    tmp3 = fmin(1.0, fmax(-1.0, tmp3));
    lat = asin(tmp3);
    xo.set(lon, lat);
} // rotateBack

void SphereDomain::
project(ProjectionType projType, const SphereCoord &xp, const SphereCoord &xo, vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            double lon, lat;
            rotate(xp, xo, lon, lat);
            double sinLon = sin(lon);
            double cosLon = cos(lon);
            double tanLat = tan(lat);
            xs[0] = _radius*cosLon/tanLat;
            xs[1] = _radius*sinLon/tanLat;
            break;
    }
} // project
 
void SphereDomain::
projectBack(ProjectionType projType, const SphereCoord &xp, SphereCoord &xo, const vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            double lon, lat;
            lon = atan2(xs[1], xs[0]);
            if (lon < 0.0) lon += PI2;
            lat = atan(_radius/sqrt(xs[0]*xs[0]+xs[1]*xs[1]));
#ifndef NDEBUG
            assert(lat > 0);
#endif
            rotateBack(xp, xo, lon, lat);
            break;
    }
} // projectBack

string SphereDomain::
brief() const {
    static string brief = "sphere domain";
    return brief;
} // brief

} // geomtk
