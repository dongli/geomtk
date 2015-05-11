#ifndef __GEOMTK_Mesh__
#define __GEOMTK_Mesh__

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
    MeshType _type;
    DomainType *_domain;
    field<double> volumes;
    bool set;
public:
    Mesh(DomainType &domain);
    virtual ~Mesh();

    /**
     *  Read grids from file.
     *
     *  @param filePath the grid file name.
     */
    virtual void
    init(const string &filePath) = 0;

    /**
     *  Read grids from one file for horizontal grids and one file for vertical
     *  grids.
     *
     *  @param fileNameH the horizontal grid file name.
     *  @param fileNameV the vertical grid file name.
     */
    virtual void
    init(const string &fileNameH, const string &fileNameV) = 0;

    /**
     *  Get the mesh type (see enum MeshType), then you can cast the mesh to
     *  the actual type.
     *
     *  @param The mesh type.
     */
    MeshType
    type() const {
        return _type;
    }

    /**
     *  Get the spatial domain.
     *
     *  @return The domain.
     */
    DomainType&
    domain() const {
        return *_domain;
    }

    /**
     *  Get the space coordinate of a grid with given location.
     *
     *  @param loc the grid location.
     *  @param i   the grid index (the cells are arranged as 1D array).
     *  @return The grid space coordinate.
     */
    virtual const CoordType&
    gridCoord(int loc, int i) const = 0;

    /**
     *  Set the center grid cell volumes.
     */
    virtual void
    setCellVolumes() = 0;

    /**
     *  Get the cell volume.
     *
     *  @param i the cell index (the cells are arranged as 1D array).
     */
    double
    cellVolume(int i) const {
        return volumes(i);
    };

    /**
     *  Get the cell size.
     *
     *  @param loc the grid location.
     *  @param i   the grid index.
     *
     *  @return The cell size array.
     */
    virtual vec
    cellSize(int loc, int i) const = 0;

    /**
     *  Get the total grid number with given location and dimension number.
     *
     *  @param loc    the cell location.
     *  @param numDim the dimension number.
     *
     *  @return The total grid number.
     */
    virtual uword
    totalNumGrid(int loc, uword numDim) const = 0;

    /**
     *  Get the vertical level index.
     *
     *  @param loc     the grid location.
     *  @param cellIdx the cell index.
     *
     *  @return The vertical level index.
     */
    virtual uword
    levelIndex(int loc, int cellIdx) const = 0;

    /**
     *  Check if the mesh is set or not.
     *
     *  @return The boolean status.
     */
    bool
    isSet() const {
        return set;
    }

    /**
     *  Check if the mesh is the same as the other one.
     *
     *  @return The boolean status.
     */
    virtual bool
    isHorizontalGridsSame(const Mesh<DomainType, CoordType> &other) const = 0;
}; // Mesh

} // geomtk

#include "Mesh-impl.h"

#endif // __GEOMTK_Mesh__
