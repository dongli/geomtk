#ifndef __RLLVelocityField__
#define __RLLVelocityField__

#include "RLLVectorField.h"

/**
 * @class PolarRing
 * @brief Polar ring class.
 * This class is for handling velocity field in Pole region.
 */

class PolarRing {
protected:
    TimeLevels<mat, 2> data[3];
    TimeLevels<mat, 2> tranData[3];
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
};

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
};

#endif
