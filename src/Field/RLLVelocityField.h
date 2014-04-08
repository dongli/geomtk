#ifndef __Geomtk_RLLVelocityField__
#define __Geomtk_RLLVelocityField__

#include "NumericRLLField.h"
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
                const vector<NumericRLLField<double, 2> > &v,
                const NumericRLLField<double, 2> &div,
                bool updateHalfLevel = false);

    double getOriginalData(int dim, const TimeLevelIndex<2> &timeIdx,
                           int i, int k = 0) const;

    double getTransformedData(int dim, const TimeLevelIndex<2> &timeIdx,
                              int i, int k = 0) const;

    void print() const;
};

// -----------------------------------------------------------------------------

class RLLVelocityField {
public:
    typedef SphereDomain DomainType;
    typedef RLLMesh MeshType;
    typedef NumericRLLField<double, 2> FieldType;
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
    const DomainType *domain;   //>! underlying domain
    const MeshType *mesh;       //>! underlying mesh
    vector<FieldType> v;        //>! velocity component fields
    FieldType div;              //>! divergence field
    PolarRing rings[2];
public:
    RLLVelocityField();
    virtual ~RLLVelocityField();

    void applyBndCond(const TimeLevelIndex<2> &timeIdx,
                      bool updateHalfLevel = false);

    virtual void create(const RLLMesh &mesh, bool useStagger,
                        bool hasHalfLevel = false);

    const Mesh& getMesh() const { return v[0].getMesh(); }

    FieldType& operator()(int compIdx) { return v[compIdx]; }
    
    const FieldType& operator()(int compIdx) const { return v[compIdx]; }

    const FieldType& getDivergence() const { return div; }

    const PolarRing& getPolarRing(Pole pole) const { return rings[pole]; }

    void calcDivergence(const TimeLevelIndex<2> &timeIdx);
};

}

#endif
