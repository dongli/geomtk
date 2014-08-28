#include "SphereDomain.h"

namespace geomtk {

SphereDomain::SphereDomain() {
    SphereDomain(2);
}

SphereDomain::SphereDomain(int numDim) : Domain(numDim) {
    type = SPHERE_DOMAIN;
    if (numDim != 2) {
        REPORT_ERROR("Spherical domain dimension should be 2 in this case!");
    }
    radius = 1.0;
    setAxis(0, "lon", "longitude", "radian_east", 0.0, PERIODIC, 2.0*M_PI, PERIODIC);
    setAxis(1, "lat", "latitude", "radian_north", -M_PI_2, POLE, M_PI_2, POLE);
}

SphereDomain::SphereDomain(VertCoordType vertCoordType) : Domain(vertCoordType) {
    type = SPHERE_DOMAIN;
    radius = 1.0;
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
            axisName[2] = "lev";
            bndTypeStarts[2] = RIGID;
            bndTypeEnds[2] = RIGID;
            // User should set the vertical pressure levels manually.
            break;
        default:
            REPORT_ERROR("Under construction!");
            break;
    }
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
    xr.setCoord(lon, lat);
}

void SphereDomain::rotate(const SphereCoord &xp, const SphereCoord &xo,
                          double &lonR, double &latR) const {
    double dlon = xo(0)-xp(0);
    double cosDlon = cos(dlon);
    double sinDlon = sin(dlon);
    
    double tmp1, tmp2, tmp3;
    
    tmp1 = xo.getCosLat()*sinDlon;
    tmp2 = xo.getCosLat()*xp.getSinLat()*cosDlon-xp.getCosLat()*xo.getSinLat();
    lonR = atan2(tmp1, tmp2);
    if (lonR < 0.0) lonR += PI2;
    
    tmp1 = xo.getSinLat()*xp.getSinLat();
    tmp2 = xo.getCosLat()*xp.getCosLat()*cosDlon;
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
}

void SphereDomain::rotateBack(const SphereCoord &xp, SphereCoord &xo,
                              const SphereCoord &xr) const {
    double tmp1, tmp2, tmp3, lon, lat;
    
    tmp1 = xr.getCosLat()*xr.getSinLon();
    tmp2 = xr.getSinLat()*xp.getCosLat()+xr.getCosLat()*xr.getCosLon()*xp.getSinLat();
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
    
    tmp1 = xr.getSinLat()*xp.getSinLat();
    tmp2 = xr.getCosLat()*xp.getCosLat()*xr.getCosLon();
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
    xo.setCoord(lon, lat);
}
    
void SphereDomain::rotateBack(const SphereCoord &xp, SphereCoord &xo,
                              double lonR, double latR) const {
    double tmp1, tmp2, tmp3, lon, lat;
    
    double sinLonR = sin(lonR);
    double cosLonR = cos(lonR);
    double sinLatR = sin(latR);
    double cosLatR = cos(latR);
    
    tmp1 = cosLatR*sinLonR;
    tmp2 = sinLatR*xp.getCosLat()+cosLatR*cosLonR*xp.getSinLat();
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
    
    tmp1 = sinLatR*xp.getSinLat();
    tmp2 = cosLatR*xp.getCosLat()*cosLonR;
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
    xo.setCoord(lon, lat);
}

void SphereDomain::project(ProjectionType projType, const SphereCoord &xp,
                           const SphereCoord &xo, vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            double lon, lat;
            rotate(xp, xo, lon, lat);
            double sinLon = sin(lon);
            double cosLon = cos(lon);
            double tanLat = tan(lat);
            xs[0] = radius*cosLon/tanLat;
            xs[1] = radius*sinLon/tanLat;
            break;
    }
}
 
void SphereDomain::projectBack(ProjectionType projType, const SphereCoord &xp,
                               SphereCoord &xo, const vec &xs) const {
    switch (projType) {
        case STEREOGRAPHIC:
            double lon, lat;
            lon = atan2(xs[1], xs[0]);
            if (lon < 0.0) lon += PI2;
            lat = atan(radius/sqrt(xs[0]*xs[0]+xs[1]*xs[1]));
#ifndef NDEBUG
            assert(lat > 0);
#endif
            rotateBack(xp, xo, lon, lat);
            break;
    }
}

string SphereDomain::getBrief() const {
    static string brief = "sphere domain";
    return brief;
}

}
