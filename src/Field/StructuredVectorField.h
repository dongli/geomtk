#ifndef __StructuredVectorField__
#define __StructuredVectorField__

#include "Field.h"
#include "StructuredMesh.h"

/**
 * StructuredVectorField
 * This class describes the vector field on the structured mesh.
 */

class StructuredVectorField : public Field {
protected:
    TimeLevels<cube, 2> data[3];
    StaggerType **staggerTypes;
public:
    StructuredVectorField(Mesh &mesh);
    virtual ~StructuredVectorField();

    /**
     * Apply the boundary conditions.
     * After the vector field is updated, set its boundary grids if any
     * according to the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel);

    /**
     * Create the memory storage of the vector field.
     * @param uXStaggerType the stagger type of the x grids of u.
     * @param uYStaggerType the stagger type of the y grids of u.
     * @param vXStaggerType the stagger type of the x grids of v.
     * @param vYStaggerType the stagger type of the y grids of v.
     */
    virtual void create(StaggerType uXStaggerType,
                        StaggerType uYStaggerType,
                        StaggerType vXStaggerType,
                        StaggerType vYStaggerType);
    /**
     * Create the memory storage of the vector field.
     * @param uXStaggerType the stagger type of the x grids of u.
     * @param uYStaggerType the stagger type of the y grids of u.
     * @param uZStaggerType the stagger type of the z grids of u.
     * @param vXStaggerType the stagger type of the x grids of v.
     * @param vYStaggerType the stagger type of the y grids of v.
     * @param vZStaggerType the stagger type of the z grids of v.
     * @param wXStaggerType the stagger type of the x grids of w.
     * @param wYStaggerType the stagger type of the y grids of w.
     * @param wZStaggerType the stagger type of the z grids of w.
     */
    virtual void create(StaggerType uXStaggerType,
                        StaggerType uYStaggerType,
                        StaggerType uZStaggerType,
                        StaggerType vXStaggerType,
                        StaggerType vYStaggerType,
                        StaggerType vZStaggerType,
                        StaggerType zXStaggerType,
                        StaggerType zYStaggerType,
                        StaggerType zZStaggerType);

    double operator()(int timeLevel, int dim, int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int dim, int i, int j, int k = 0);

    virtual StaggerType getStaggerType(int comp, int dim) const;
};

#endif
