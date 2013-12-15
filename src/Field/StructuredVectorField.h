#ifndef __StructuredVectorField__
#define __StructuredVectorField__

#include "Field.h"
#include "StructuredMesh.h"

namespace geomtk {

/**
 * StructuredVectorField
 * This class describes the vector field on the structured mesh.
 */

class StructuredVectorField : public Field {
protected:
    TimeLevels<cube, 2> *data[3];
    ArakawaGrid gridType;
    StaggerType **staggerTypes;
public:
    StructuredVectorField(const Mesh &mesh, bool hasHalfLevel = false);
    StructuredVectorField(const string &name, const string &units,
                          const string &longName, const Mesh &mesh,
                          bool hasHalfLevel = false);
    virtual ~StructuredVectorField();

    /**
     * Apply the boundary conditions.
     * After the vector field is updated, set its boundary grids if any
     * according to the boundary conditions.
     * @param timeLevel the time level.
     */
    virtual void applyBndCond(int timeLevel, bool updateHalfLevel = false);

    virtual void create(ArakawaGrid gridType);
    
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

    /**
     * Convert field from current grid type to the given type.
     * Typical usage is conversion from C grid to A grid and vice verse.
     * @param gridType the grid type for conversion.
     * @param timeLevel the time level.
     * @param u output of u component.
     * @param v output of v component.
     * @param w output of w component (only in 3D).
     */
    virtual void convert(ArakawaGrid gridType, int timeLevel, cube &u, cube &v);
    virtual void convert(ArakawaGrid gridType, int timeLevel,
                         cube u, cube v, cube w);

    double operator()(int timeLevel, int dim, int i, int j, int k = 0) const;
    double& operator()(int timeLevel, int dim, int i, int j, int k = 0);

    virtual StaggerType getStaggerType(int comp, int dim) const;
};

}

#endif
