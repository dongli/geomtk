#ifndef __StructuredScalarField__
#define __StructuredScalarField__

#include "Field.h"

namespace geomtk {

class StructuredScalarField : public Field {
protected:
    TimeLevels<cube, 2> data;
    StaggerType *staggerTypes;
public:
    StructuredScalarField(Mesh &mesh);
    virtual ~StructuredScalarField();

    /**
     * Apply the boundary conditions.
     * After the scalar field is updated, set its boundary grids if any
     * according to the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel);

    /**
     * Create the memory storage for the scalar field.
     * @param xStaggerType the stagger type of x grids.
     * @param yStaggerType the stagger type of y grids.
     */
    virtual void create(StaggerType xStaggerType,
                        StaggerType yStaggerType);

    /**
     * Create the memory for the scalar field.
     * @param xStaggerType the stagger type of x grids.
     * @param yStaggerType the stagger type of y grids.
     * @param zStaggerType the stagger type of z grids.
     */
    virtual void create(StaggerType xStaggerType,
                        StaggerType yStaggerType,
                        StaggerType zStaggerType);

    /**
     * Subscript operator of scalar field.
     * @param timeLevel the time level.
     * @param i the index of x dimension.
     * @param j the index of y dimension.
     * @param k the index of z dimension.
     * @return The scalar on the given indices.
     */
    double operator()(int timeLevel, int i, int j, int k = 0) const;

    /**
     * Subscript operator of scalar field.
     * @param timeLevel the time level.
     * @param i the index of x dimension.
     * @param j the index of y dimension.
     * @param k the index of z dimension.
     * @return The scalar on the given indices.
     */
    double& operator()(int timeLevel, int i, int j, int k = 0);

    virtual StaggerType getStaggerType(int dim) const;
};

}

#endif
