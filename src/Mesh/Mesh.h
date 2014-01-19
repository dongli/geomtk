#ifndef __Geomtk_Mesh__
#define __Geomtk_Mesh__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

enum StaggerType {
    CENTER, EDGE, VERTEX
};

enum ArakawaGrid {
    A_GRID, B_GRID, C_GRID, D_GRID, E_GRID
};

class MeshIndex;

class Mesh {
protected:
    Domain *domain;
    bool set;
public:
    Mesh(Domain &domain);
    virtual ~Mesh();

    /**
     *  Get the spatial domain.
     *
     *  @return The domain.
     */
    const Domain& getDomain() const;

    /**
     *  Get the total number of grids of a given grid type no matter the
     *  underlying mesh type.
     *
     *  @param gridType the Arakawa grid stagger type.
     *  @param dim      the optional dimension index for non-A_GRID grids.
     *
     *  @return The total number of grids.
     */
    virtual int getTotalNumGrid(ArakawaGrid gridType, int dim = 0) const = 0;

    /**
     *  Get the grid coordinate of a given mesh index and of a given grid type.
     *
     *  @param idx      the mesh index.
     *  @param x        the outputted grid coordinate.
     *  @param gridType the Arakawa grid stagger type.
     *  @param dim      the dimension for non-A-grid.
     */
    virtual void getGridCoord(const MeshIndex &idx, SpaceCoord &x,
                              ArakawaGrid gridType, int dim = 0) const = 0;

    /**
     *  Get the grid coordinate of a given mesh index and of a given grid type.
     *
     *  @param idx      the mesh index.
     *  @param x        the outputted grid coordinate.
     *  @param gridType the Arakawa grid stagger type.
     *  @param dim      the dimension for non-A-grid.
     */
    virtual void getGridCoord(int idx, SpaceCoord &x,
                              ArakawaGrid gridType, int dim = 0) const = 0;

    /**
     *  Set the center grid cell volumes.
     */
    virtual void setCellVolumes() = 0;

    /**
     *  Get the center grid cell volume.
     *
     *  @param idx      the mesh index.
     *  @param volume   the outputted cell volume.
     */
    virtual void getCellVolume(int idx, double &area) const = 0;

    /**
     *  Check if the mesh is set or not.
     *
     *  @return The boolean flag.
     */
    bool isSet() const;
};

}

#endif
