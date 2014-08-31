#ifndef __Geomtk_Mesh__
#define __Geomtk_Mesh__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

enum MeshType {
    CARTESIAN_MESH, RLL_MESH
};

/**
 *  This is an abstract class.
 */
template <class DomainType_, class CoordType>
class Mesh {
public:
    typedef DomainType_ DomainType;
protected:
    MeshType type;
    DomainType *domain;
    bool set;
public:
    Mesh(DomainType &domain);
    virtual ~Mesh();

    /**
     *  Read grids from file.
     *
     *  @param fileName the grid file name.
     */
    virtual void init(const string &fileName);

    /**
     *  Read grids from one file for horizontal grids and one file for vertical
     *  grids.
     *
     *  @param fileNameH the horizontal grid file name.
     *  @param fileNameV the vertical grid file name.
     */
    virtual void init(const string &fileNameH, const string &fileNameV);

    /**
     *  Get the mesh type (see enum MeshType), then you can cast the mesh to
     *  the actual type.
     *
     *  @param The mesh type.
     */
    MeshType getType() const { return type; }

    /**
     *  Get the spatial domain.
     *
     *  @return The domain.
     */
    DomainType& getDomain() const { return *domain; }

    /**
     *  Get the space coordinate of a grid with given location.
     *
     *  @param i   the grid index (the cells are arranged as 1D array).
     *  @param loc the grid location.
     *  @param x   the grid space coordinate.
     */
    virtual void getGridCoord(int i, int loc, CoordType &x) const = 0;

    /**
     *  Set the center grid cell volumes.
     */
    virtual void setCellVolumes() = 0;

    /**
     *  Get the cell volume.
     *
     *  @param i the cell index (the cells are arranged as 1D array).
     */
    virtual double getCellVolume(int i) const = 0;

    /**
     *  Get the total grid number with given location and dimension number.
     *
     *  @param loc    the cell location.
     *  @param numDim the dimension number.
     *
     *  @return The total grid number.
     */
    virtual int getTotalNumGrid(int loc, int numDim) const = 0;

    /**
     *  Get the level index.
     *
     *  @param i   the cell index (the cells are arranged as 1D array).
     *  @param loc the cell location.
     */
    virtual int getLevelIndex(int i, int loc) const;

    /**
     *  Check if the mesh is set or not.
     *
     *  @return The boolean status.
     */
    bool isSet() const { return set; }

    /**
     *  Check if the mesh is the same as the other one.
     *
     *  @return The boolean status.
     */
    virtual bool isHorizontalGridsSame(const Mesh<DomainType, CoordType> &other) const = 0;
};

} // geomtk

#include "Mesh-impl.h"

#endif // __Geomtk_Mesh__
