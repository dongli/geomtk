#ifndef __Geomtk_StructuredMesh__
#define __Geomtk_StructuredMesh__

#include "Mesh.h"

namespace geomtk {
    
class StructuredMeshIndex;

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
     *  Set the grid coordinates along an axis.
     *
     *  This method sets the grid coordinates including the full grids and the
     *  half grids. The internal grids will be set according to the domain
     *  boundary conditions.
     *
     *  @param dim  the dimension index of the axis.
     *  @param size the size of the full grids.
     *  @param full the full grid coordinates.
     *  @param half the half grid coordinates.
     */
    virtual void setGridCoords(int dim, int size, double *full, double *half);

    /**
     *  Set the grid coordinates along an axis.
     *
     *  @param dim      the dimension index of the axis.
     *  @param fileName the file (netCDF) name that contains grid coordinates.
     *  @param fullName the variable name of the full coordinates.
     *  @param halfName the variable name of the half coordinates.
     */
    virtual void setGridCoords(int dim, const string &fileName,
                               const string &fullName, const string &halfName);

    /**
     *  Return the grid coordinates along an axis.
     *
     *  This method returns the grid coordinates excluding the potential virtual
     *  boundary grids. The virtual boundary grids are added when boundary
     *  conditions are periodic.
     *
     *  @param dim             the dimension index.
     *  @param staggerType     the grid stagger type.
     *  @param hasVirtualGrids flag for enclosing virtual boundary grids.
     *
     *  @return The grid coordinates.
     */
    vec getGridCoords(int dim, StaggerType staggerType,
                      bool hasVirtualGrids = false) const;

    /**
     *  Return the grid coordinate along an axis and with a given grid index.
     *
     *  @param dim         the dimension index.
     *  @param staggerType the grid stagger type.
     *  @param i           the grid index.
     *
     *  @return The grid coordinate.
     */
    double getGridCoord(int dim, StaggerType staggerType, int i) const;

    /**
     *  Return the coordinate of specific grid.
     *
     *  @param staggerType the grid stagger type.
     *  @param idx         the mesh index.
     *  @param x           the grid coordinate.
     */
    void getGridCoord(StaggerType staggerType, const StructuredMeshIndex &idx,
                      SpaceCoord &x) const;

    /**
     *  Return the grid interval along an axis and with a given grid index.
     *
     *  @param dim         the dimension index.
     *  @param staggerType the grid stagger type.
     *  @param i           the grid index.
     *
     *  @return The grid interval.
     */
    double getGridInterval(int dim, StaggerType staggerType, int i) const;

    /**
     *  Return the grid number.
     *
     *  This method returns the grid number excluding the potential virtual
     *  boundary grids. The virtual boundary grids are added when boundary
     *  conditions are periodic.
     *
     *  @param dim             the dimension index (start from zero).
     *  @param staggerType     the grid stagger type.
     *  @param hasVirtualGrids flag for enclosing virtual boundary grids.
     *
     *  @return The grid number.
     */
    int getNumGrid(int dim, StaggerType staggerType,
                   bool hasVirtualGrids = false) const;

    /**
     *  Inquire if the grids along an axis is equidistant or not.
     *
     *  @param dim the dimension index (start from zero).
     *
     *  @return A boolean.
     */
    bool isEquidistant(int dim) const;
};

}

#endif
