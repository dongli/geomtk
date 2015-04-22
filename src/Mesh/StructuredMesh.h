#ifndef __GEOMTK_StructuredMesh__
#define __GEOMTK_StructuredMesh__

#include "Mesh.h"

namespace geomtk {

/**
 *  This class describes the grid stagger in structured mesh.
 */
struct StructuredStagger {
    struct Location {
        static const int CENTER = 0;
        static const int X_FACE = 1;
        static const int Y_FACE = 2;
        static const int XY_VERTEX = 3;
        static const int Z_FACE = 4;
    };
    struct GridType {
        static const int FULL = 0;
        static const int HALF = 1;
    };
};

enum StructuredGridStyle {
    FULL_LEAD,
    HALF_LEAD
};

/**
 *  This class describes the structured mesh by storing the grid coordinates and
 *  providing several querying methods.
 */
template <class DomainType, class CoordType>
class StructuredMesh : public Mesh<DomainType, CoordType> {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    int _haloWidth;
    umat fullIndexRanges;
    umat halfIndexRanges;
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
    field<CoordType> gridCoords[5]; // With different locations.

    StructuredGridStyle gridStyles[3];
public:
    StructuredMesh(DomainType &domain, int haloWidth = 1);
    virtual ~StructuredMesh();

    /**
     *  Read grids from file.
     *
     *  @param filePath the grid file name.
     */
    virtual void
    init(const string &filePath);

    /**
     *  Read grids from one file for horizontal grids and one file for vertical
     *  grids.
     *
     *  @param fileNameH the horizontal grid file name.
     *  @param fileNameV the vertical grid file name.
     */
    virtual void
    init(const string &filePathH, const string &filePathV);

    virtual void
    init(int nx, int ny = 1, int nz = 1);

    int
    haloWidth() const {
        return _haloWidth;
    }

    /**
     *  Set the grid index ranges (start index and end index).
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type (FULL or HALF).
     *  @param start    the start index.
     *  @param end      the end index.
     */
    void
    setGridIndexRange(int axisIdx, int gridType, int start, int end);

    /**
     *  Set the grid coordinate components.
     *
     *  @param axisIdx the axis index.
     *  @param size    the grid size.
     *  @param full    the full grid coordinate component array.
     *  @param half    the half grid coordinate component array.
     */
    virtual void
    setGridCoordComps(int axisIdx, int size, const vec &full, const vec &half);

    /**
     *  Set the grid coordinate components.
     *
     *  @param axisIdx the axis index.
     *  @param size    the grid size.
     *  @param full    the full grid coordinate component array.
     */
    virtual void
    setGridCoordComps(int axisIdx, int size, const vec &full);

    /**
     *  Get the grid coordinate component array.
     *
     *  @param axisIdx      the axis index.
     *  @param gridType     the grid type (FULL or HALF).
     *  @param hasHaloGrids boolean flag of including virtual grids.
     *
     *  @return The grid coordinate component array.
     */
    vec
    gridCoordComps(int axisIdx, int gridType, bool hasHaloGrids = false) const;

    /**
     *  Get the grid coordinate component at given position.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type (FULL or HALF).
     *  @param gridIdx  the grid index.
     *
     *  @return The grid coordinate component.
     */
    double
    gridCoordComp(int axisIdx, int gridType, int gridIdx) const;

    /**
     *  Get the space coordinate of a grid with given location.
     *
     *  @param loc the grid location.
     *  @param i   the cell index or the grid index along x axis in 1D domain.
     *
     *  @return The grid space coordinate.
     */
    virtual const CoordType&
    gridCoord(int loc, int i) const;

    virtual const CoordType&
    gridCoord(int loc, int i, int j, int k = 0) const;

    /**
     *  Set the cell volumes after grids are set.
     */
    virtual void
    setCellVolumes() = 0;

    /**
     *  Get the grid interval.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type (FULL or HALF).
     *  @param gridIdx  the grid index.
     *
     *  @return The grid interval.
     */
    double
    gridInterval(int axisIdx, int gridType, int gridIdx) const;

    /**
     *  Get the total grid number on given location (e.g. CENTER).
     *
     *  @param loc    the grid location.
     *  @param numDim the dimension number.
     *
     *  @return The total grid number.
     */
    virtual int
    totalNumGrid(int loc, int numDim = -1) const;

    /**
     *  Get the grid number along the given axis.
     *
     *  @param axisIdx      the axis index.
     *  @param gridType     the grid type.
     *  @param hasHaloGrids boolean flag of including virtual grids.
     *
     *  @return The grid number.
     */
    int
    numGrid(int axisIdx, int gridType, bool hasHaloGrids = false) const;

    /**
     *  Get the start grid index along the given axis.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type.
     *
     *  @return The start grid index.
     */
    int
    startIndex(int axisIdx, int gridType) const;

    /**
     *  Get the end grid index along the given axis.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type.
     *
     *  @return The end grid index.
     */
    int
    endIndex(int axisIdx, int gridType) const;

    /**
     *  Get the start grid index along x axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along x axis.
     */
    int
    is(int gridType) const;

    /**
     *  Get the end grid index along x axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along x axis.
     */
    int
    ie(int gridType) const;

    /**
     *  Get the start grid index along y axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along x axis.
     */
    int
    js(int gridType) const;

    /**
     *  Get the end grid index along y axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along y axis.
     */
    int
    je(int gridType) const;

    /**
     *  Get the start grid index along z axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along z axis.
     */
    int
    ks(int gridType) const;

    /**
     *  Get the end grid index along z axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along z axis.
     */
    int
    ke(int gridType) const;

    /**
     *  Get the vertical level index.
     *
     *  @param loc     the grid location.
     *  @param cellIdx the cell index.
     *
     *  @return The vertical level index.
     */
    virtual int
    levelIndex(int loc, int cellIdx) const;

    virtual void
    unwrapIndex(int loc, int cellIdx, int gridIdx[3]) const;

    virtual int
    wrapIndex(int loc, int i, int j = 0, int k = 0) const;

    virtual bool
    isHorizontalGridsSame(const Mesh<DomainType, CoordType> &other) const;
protected:
    virtual void
    setGridCoords();
}; // StructuredMesh

} // geomtk

#include "StructuredMesh-impl.h"

#endif // __GEOMTK_StructuredMesh__
