#ifndef __Geomtk_ClassicPressureSigma__
#define __Geomtk_ClassicPressureSigma__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

/**
 *  This class describes the classic pressure sigma vertical coordinate:
 *
 *               p  - pt
 *      sigma = ---------
 *               ps - pt
 *
 *  where p is the pressure, pt is the model top pressure and ps is the
 *  surface pressure.
 */
class ClassicPressureSigma : public VertCoord {
public:
    vec fullSigma;
    vec halfSigma;
    double pt;

    ClassicPressureSigma();
    virtual ~ClassicPressureSigma();

    double getPressure(double ps, int k) const;
};

}

#endif