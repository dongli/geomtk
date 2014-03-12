#ifndef __Geomtk_RLLVelocityField__
#define __Geomtk_RLLVelocityField__

#include "RLLField.h"
#include "SphereDomain.h"

namespace geomtk {

/**
 *  This class is for handling velocity interpolating in Pole region, since the
 *  direction of velocity is singular on Poles. The zonal grids nearest to Poles
 *  compose a "ring".  The velocity on this ring is reorganized onto A-grid
 *  position, and is transformed onto a polar stereographic plane.  When
 *  interpolating a point enclosed by the ring, the transformed velocity is
 *  interpolated onto the point by using the inverse distance weighting.
 */
class PolarRing {
protected:
    field<TimeLevels<SphereVelocity, 2>*> vr;
    const RLLMesh *mesh;
public:
    PolarRing();
    ~PolarRing();

    void create(const RLLMesh &mesh, bool hasHalfLevel = false);

    void update(const TimeLevelIndex<2> &timeIdx, Pole pole,
                const field<TimeLevels<field<double>, 2>*> &data,
                bool updateHalfLevel = false);

    double getOriginalData(int dim, const TimeLevelIndex<2> &timeIdx,
                           int i, int k = 0) const;

    double getTransformedData(int dim, const TimeLevelIndex<2> &timeIdx,
                              int i, int k = 0) const;

    void print() const;
};

// -----------------------------------------------------------------------------

class RLLVelocityField : public RLLField<double> {
protected:
    PolarRing rings[2];
public:
    RLLVelocityField();
    virtual ~RLLVelocityField();

    void applyBndCond(const TimeLevelIndex<2> &timeIdx,
                      bool updateHalfLevel = false);

    virtual void create(const string &name, const string &units,
                        const string &longName, const RLLMesh &mesh,
                        int numDim, ArakawaGrid gridType,
                        bool hasHalfLevel = false);

    const PolarRing& getPolarRing(Pole pole) const;
};

}

#endif
