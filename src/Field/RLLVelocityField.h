#ifndef __RLLVelocityField__
#define __RLLVelocityField__

#include "RLLVectorField.h"
#include "SphereDomain.h"

namespace geomtk {

/**
 * PolarRing
 * This class is for handling velocity interpolating in Pole region, since the
 * direction of velocity is singular on Poles. The zonal grids nearest to Poles
 * compose a "ring".  The velocity on this ring is reorganized onto A-grid
 * position, and is transformed onto a polar stereographic plane.  When
 * interpolating a point enclosed by the ring, the transformed velocity is
 * interpolated onto the point by using the inverse distance weighting.
 */

class PolarRing {
protected:
    TimeLevels<SphereVelocity, 2> ***vr;
    const RLLMesh *mesh;
public:
    PolarRing();
    ~PolarRing();

    /**
     * Create the memory storage for the polar ring velocity.
     * The velocity on C-grid position is reorganized onto A-grid, and is
     * transformed onto a polar stereographic plane.
     * @param mesh The underlying mesh.
     */
    void create(const Mesh &mesh, bool hasHalfLevel = false);

    /**
     * Update velocity on the polar ring.
     * When the host velocity field is updated (aka.  applyBndCond is invoked),
     * update the associated polar ring velocity.
     * @param timeLevel the time level to be updated.
     * @param pole which Pole the polar ring is at.
     * @param data the host field data.
     */
    void update(int timeLevel, Pole pole, TimeLevels<cube, 2> **data,
                bool updateHalfLevel = false);

    /**
     * Return the original velocity.
     * @param timeLevel the time level to be updated.
     * @param dim which dimension of velocity that is needed.
     * @param i the longitude index of ring grids.
     * @param k the level index of ring grids.
     */
    double getOriginalData(int timeLevel, int dim, int i, int k = 0) const;

    /**
     * Return the transformed velocity.
     * @param timeLevel the time level to be updated.
     * @param dim which dimension of velocity that is needed.
     * @param i the longitude index of ring grids.
     * @param k the level index of ring grids.
     */
    double getTransformedData(int timeLevel, int dim, int i, int k = 0) const;

    void print() const;
};

// -----------------------------------------------------------------------------

/**
 * RLLVelocityField
 * This class describes the velocity field on the RLL mesh. It is derived from
 * vector field because of the singularity of velocity direction on Poles. This
 * will cause problems when interpolating velocity near Poles.
 */

class RLLVelocityField : public RLLVectorField {
protected:
    PolarRing rings[2];
private:
    bool hasHalfLevel;
public:
    RLLVelocityField(const Mesh &mesh, bool hasHalfLevel = false);
    RLLVelocityField(const string &name, const string &units,
                     const string &longName, const Mesh &mesh,
                     bool hasHalfLevel = false);
    virtual ~RLLVelocityField();

    /**
     * Apply the boundary conditions.
     * After the velocity field is updated, set its boundary grids if any
     * according to the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel, bool updateHalfLevel = false);

    virtual void create(ArakawaGrid gridType);

    /**
     * Create the memory storage for the velocity field.
     * @param uLonStaggerType the stagger type of longitude grids of u.
     * @param uLatStaggerType the stagger type of latitude grids of u.
     * @param vLonStaggerType the stagger type of longitude grids of v.
     * @param vLatStaggerType the stagger type of latitude grids of v.
     */
    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType);

    /**
     * Create the memory storage for the velocity field.
     * @param uLonStaggerType the stagger type of longitude grids of u.
     * @param uLatStaggerType the stagger type of latitude grids of u.
     * @param uLevStaggerType the stagger type of level grids of u.
     * @param vLonStaggerType the stagger type of longitude grids of v.
     * @param vLatStaggerType the stagger type of latitude grids of v.
     * @param vLevStaggerType the stagger type of level grids of v.
     * @param wLonStaggerType the stagger type of longitude grids of w.
     * @param wLatStaggerType the stagger type of latitude grids of w.
     * @param wLevStaggerType the stagger type of level grids of w.
     */
    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType uLevStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType,
                        StaggerType vLevStaggerType,
                        StaggerType wLonStaggerType,
                        StaggerType wLatStaggerType,
                        StaggerType wLevStaggerType);

    /**
     * Return the polar ring at the given Pole.
     * @param pole the Pole.
     * @return Polar ring object.
     */
    virtual const PolarRing& getPolarRing(Pole pole) const;
};

}

#endif
