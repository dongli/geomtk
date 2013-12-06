#ifndef __StructuredMesh__
#define __StructuredMesh__

#include "Mesh.h"

class StructuredMesh : public Mesh {
protected:
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
    bool *equidistant;
public:
    StructuredMesh(Domain &domain);
    virtual ~StructuredMesh();

    /**
     * @brief A setter for grid coordinates.
     * This method sets the grid coordinates including the full grids and the
     * half grids. The internal grids will be set according to the domain
     * boundary conditions.
     * @param dim the dimension index.
     * @param size the size of full grids (maybe also for half).
     * @param full the full grid coordinates.
     * @param half the half grid coordinates.
     */
    virtual void setGridCoords(int dim, int size, double *full, double *half);

    /**
     * @brief A getter for grid coordinates.
     * This method returns the grid coordinates excluding the potential virtual
     * boundary grids. The virtual boundary grids are added when boundary
     * conditions are periodic.
     * @param dim the dimension index.
     * @param staggerType the grid stagger type.
     * @param hasVirtualGrids flag for enclosing virtual boundary grids.
     * @return The grid coordinates
     */
    vec getGridCoords(int dim, StaggerType staggerType,
                      bool hasVirtualGrids = false) const;

    double getGridCoord(int dim, StaggerType staggerType, int i) const;

    /**
     * @brief A getter for grid number.
     * This method returns the grid number excluding the potential virtual
     * boundary grids. The virtual boundary grids are added when boundary
     * conditions are periodic.
     * @param dim the dimension index.
     * @param staggerType the grid stagger type.
     * @param hasVirtualGrids flag for enclosing virtual boundary grids.
     * @return The grid number
     */
    int getNumGrid(int dim, StaggerType staggerType,
                   bool hasVirtualGrids = false) const;

    bool isEquidistant(int dim) const;
};

#endif
