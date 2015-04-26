#ifndef __GEOMTK_StructuredMesh__
#define __GEOMTK_StructuredMesh__

#include "Mesh.h"

namespace geomtk {

/**
 *  This class describes the grid stagger in structured mesh.
 *
 *  Pair relationship:
 *
 *      CENTER - VERTEX
 *      X_FACE - Y_FACE/YZ_VERTEX
 *      Y_FACE - X_FACE/XZ_VERTEX
 *      Z_FACE - XY_VERTEX
 */
struct StructuredStagger {
    struct Location {
        static const int CENTER = 0;
        static const int VERTEX = 1;
        static const int X_FACE = 2;
        static const int Y_FACE = 3;
        static const int Z_FACE = 4;
        static const int XY_VERTEX = 5;
        static const int XZ_VERTEX = 6;
        static const int YZ_VERTEX = 7;
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
class StructuredMesh
: public Mesh<DomainType, CoordType> {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    uword _haloWidth;
    arma::Mat<int> fullIndexRanges;
    arma::Mat<int> halfIndexRanges;
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
    field<CoordType> gridCoords[8]; // With different locations.

    StructuredGridStyle gridStyles[3];
public:
    StructuredMesh(DomainType &domain, uword haloWidth = 1);
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
    init(uword nx, uword ny = 1, uword nz = 1);

    uword
    haloWidth() const {
        return _haloWidth;
    }

    StructuredGridStyle
    gridStyle(int axisIdx) const {
        return gridStyles[axisIdx];
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
    setGridIndexRange(uword axisIdx, int gridType, int start, int end);

    /**
     *  Set the grid coordinate components.
     *
     *  @param axisIdx the axis index.
     *  @param size    the grid size.
     *  @param full    the full grid coordinate component array.
     *  @param half    the half grid coordinate component array.
     */
    virtual void
    setGridCoordComps(uword axisIdx, uword size,
                      const vec &full, const vec &half);

    /**
     *  Set the grid coordinate components.
     *
     *  @param axisIdx the axis index.
     *  @param size    the grid size.
     *  @param full    the full grid coordinate component array.
     */
    virtual void
    setGridCoordComps(uword axisIdx, uword size, const vec &full);

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
    gridCoordComps(uword axisIdx, int gridType,
                   bool hasHaloGrids = false) const;

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
    gridCoordComp(uword axisIdx, int gridType, int gridIdx) const;

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
    gridInterval(uword axisIdx, int gridType, int gridIdx) const;

    /**
     *  Get the total grid number on given location (e.g. CENTER).
     *
     *  @param loc    the grid location.
     *  @param numDim the dimension number.
     *
     *  @return The total grid number.
     */
    virtual uword
    totalNumGrid(int loc, uword numDim = 0) const;

    /**
     *  Get the grid number along the given axis.
     *
     *  @param axisIdx      the axis index.
     *  @param gridType     the grid type.
     *  @param hasHaloGrids boolean flag of including virtual grids.
     *
     *  @return The grid number.
     */
    uword
    numGrid(uword axisIdx, int gridType, bool hasHaloGrids = false) const;

    /**
     *  Get the start grid index along the given axis.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type.
     *
     *  @return The start grid index.
     */
    uword
    startIndex(uword axisIdx, int gridType) const;

    /**
     *  Get the end grid index along the given axis.
     *
     *  @param axisIdx  the axis index.
     *  @param gridType the grid type.
     *
     *  @return The end grid index.
     */
    uword
    endIndex(uword axisIdx, int gridType) const;

    /**
     *  Get the start grid index along x axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along x axis.
     */
    uword
    is(int gridType) const;

    /**
     *  Get the end grid index along x axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along x axis.
     */
    uword
    ie(int gridType) const;

    /**
     *  Get the start grid index along y axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along x axis.
     */
    uword
    js(int gridType) const;

    /**
     *  Get the end grid index along y axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along y axis.
     */
    uword
    je(int gridType) const;

    /**
     *  Get the start grid index along z axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The start grid index along z axis.
     */
    uword
    ks(int gridType) const;

    /**
     *  Get the end grid index along z axis.
     *
     *  @param gridType the grid type.
     *
     *  @return The end grid index along z axis.
     */
    uword
    ke(int gridType) const;

    /**
     *  Get the vertical level index.
     *
     *  @param loc     the grid location.
     *  @param cellIdx the cell index.
     *
     *  @return The vertical level index.
     */
    virtual uword
    levelIndex(int loc, int cellIdx) const;

    virtual void
    unwrapIndex(int loc, int cellIdx, vector<int> &spanIdx) const;

    virtual void
    unwrapIndex(int loc, int cellIdx, int &i) const;

    /**
     *  Unwrap the cell index (1D) into the span index (2D).
     *
     *  @param loc     the grid location.
     *  @param cellIdx the cell index.
     *  @param i       the span index of the first axis.
     *  @param j       the span index of the second axis.
     */
    virtual void
    unwrapIndex(int loc, int cellIdx, int &i, int &j) const;

    /**
     *  Unwrap the cell index (1D) into the span index (3D).
     *
     *  @param loc     the grid location.
     *  @param cellIdx the cell index.
     *  @param i       the span index of the first axis.
     *  @param j       the span index of the second axis.
     *  @param k       the span index of the third axis.
     */
    virtual void
    unwrapIndex(int loc, int cellIdx, int &i, int &j, int &k) const;

    /**
     *  Wrap the span index (1D) into cell index (1D).
     *
     *  @param loc the grid location.
     *  @param i   the span index of the first axis.
     *
     *  @return The cell index.
     */
    virtual int
    wrapIndex(int loc, int i) const;

    /**
     *  Wrap the span indices (2D) into cell index (1D).
     *
     *  @param loc the grid location.
     *  @param i   the span index of the first axis.
     *  @param j   the span index of the second axis.
     *
     *  @return The cell index.
     */
    virtual int
    wrapIndex(int loc, int i, int j) const;

    /**
     *  Wrap the span indices (3D) into cell index (1D).
     *
     *  @param loc the grid location.
     *  @param i   the span index of the first axis.
     *  @param j   the span index of the second axis.
     *  @param k   the span index of the third axis.
     *
     *  @return The cell index
     */
    virtual int
    wrapIndex(int loc, int i, int j, int k) const;

    virtual bool
    isHorizontalGridsSame(const Mesh<DomainType, CoordType> &other) const;
protected:
    virtual void
    setGridCoords();
}; // StructuredMesh

} // geomtk

#include "StructuredMesh-impl.h"

#endif // __GEOMTK_StructuredMesh__
