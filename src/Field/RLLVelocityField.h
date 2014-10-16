#ifndef __Geomtk_RLLVelocityField__
#define __Geomtk_RLLVelocityField__

#include "RLLField.h"
#include "SphereDomain.h"
#include "StructuredVelocityField.h"

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
public:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
    field<TimeLevels<SphereVelocity, 2>*> vr;
    field<TimeLevels<double, 2>*> divr;
    const RLLMesh *mesh;
public:
    PolarRing();
    ~PolarRing();

    void create(const RLLMesh &mesh, bool hasHalfLevel = false);

    void update(const TimeLevelIndex<2> &timeIdx, Pole pole,
                const vector<RLLField<double, 2> > &v,
                const RLLField<double, 2> &div,
                bool updateHalfLevel = false);

    double originalData(int dim, const TimeLevelIndex<2> &timeIdx,
                           int i, int k = 0) const;

    double transformedData(int dim, const TimeLevelIndex<2> &timeIdx,
                              int i, int k = 0) const;

    void print() const;
};

// -----------------------------------------------------------------------------

class RLLVelocityField
: public StructuredVelocityField<RLLMesh, RLLField<double, 2> > {
protected:
    PolarRing rings[2];
public:
    RLLVelocityField();
    virtual ~RLLVelocityField();

    void applyBndCond(const TimeLevelIndex<2> &timeIdx,
                      bool updateHalfLevel = false);

    virtual void create(const RLLMesh &mesh, bool useStagger,
                        bool hasHalfLevel = false);

    const PolarRing& getPolarRing(Pole pole) const { return rings[pole]; }

    virtual void calcDivergence(const TimeLevelIndex<2> &timeIdx);

    virtual void calcVorticity(const TimeLevelIndex<2> &timeIdx);
};

} // geomtk

#endif // __Geomtk_RLLVelocityField__
