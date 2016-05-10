#ifndef __GEOMTK_ClassicPressureSigma__
#define __GEOMTK_ClassicPressureSigma__

#include "geomtk_commons.h"
#include "HybridPressureSigma.h"

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
class ClassicPressureSigma : public HybridPressureSigma {
public:
    vec sigma[2];
    double pt;

    ClassicPressureSigma();
    virtual ~ClassicPressureSigma();
}; // ClassicPressureSigma

} // geomtk

#endif // __GEOMTK_ClassicPressureSigma__
