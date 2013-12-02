#ifndef __RLLVelocityField__
#define __RLLVelocityField__

#include "RLLVectorField.h"
#include "SphereDomain.h"

/**
 * @class PolarRing
 * @brief Polar ring class.
 * This class is for handling velocity field in Pole region.
 */

class PolarRing {
protected:
    TimeLevels<SphereVelocity, 2> **vr;
    RLLMesh *mesh;
public:
    PolarRing();
    ~PolarRing();

    void create(Mesh &mesh);

    /**
     * @brief Update polar ring variables.
     * When the host field is updated (applyBndCond is invoked), update the
     * associated polar ring variables.
     * @param timeLevel time level to be updated.
     * @param pole which Pole the polar ring is at.
     * @param data the host field data.
     */
    void update(int timeLevel, Pole pole, TimeLevels<cube, 2> *data);

    double getOriginalData(int timeLevel, int dim, int i, int k) const;
    double getTransformedData(int timeLevel, int dim, int i, int k) const;
};

// -----------------------------------------------------------------------------

class RLLVelocityField : public RLLVectorField {
protected:
    PolarRing *rings;
public:
    RLLVelocityField(Mesh &mesh);
    virtual ~RLLVelocityField();

    virtual void applyBndCond(int timeLevel);

    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType);
    virtual void create(StaggerType uLonStaggerType,
                        StaggerType uLatStaggerType,
                        StaggerType uLevStaggerType,
                        StaggerType vLonStaggerType,
                        StaggerType vLatStaggerType,
                        StaggerType vLevStaggerType,
                        StaggerType wLonStaggerType,
                        StaggerType wLatStaggerType,
                        StaggerType wLevStaggerType);

    virtual const PolarRing& getPolarRing(Pole pole) const;
};

#endif
