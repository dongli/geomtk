#ifndef __Geomtk_HybridPressureSigma__
#define __Geomtk_HybridPressureSigma__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

/**
 *  This class describes the hybrid pressure-sigma vertical coordinate:
 *
 *
 *      p = A * p0 + B * ps
 *
 *  where A and B are coefficients, p0 is some reference pressure and
 *  ps is the surface pressure.
 */
class HybridPressureSigma : public VertCoord {
public:
    vec fullA, halfA;
    vec fullB, halfB;
    double p0;

    HybridPressureSigma();
    virtual ~HybridPressureSigma();

    double getPressure(double ps, int k);
};

}

#endif