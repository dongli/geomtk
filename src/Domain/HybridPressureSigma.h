#ifndef __Geomtk_HybridPressureSigma__
#define __Geomtk_HybridPressureSigma__

#include "geomtk_commons.h"
#include "TerrainFollowing.h"

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
class HybridPressureSigma : public TerrainFollowing {
public:
    vec fullA, halfA;
    vec fullB, halfB;
    double p0;

    HybridPressureSigma();
    virtual ~HybridPressureSigma();

    virtual double calcPressure(double ps, int k) const;
};

}

#endif