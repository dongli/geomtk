#ifndef __Geomtk_StructuredMesh__
#define __Geomtk_StructuredMesh__

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
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
    field<double> volumes;
    field<CoordType> gridCoords[5]; // With different locations.
public:
    StructuredMesh(DomainType &domain);
    virtual ~StructuredMesh();

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

    virtual void init(int nx, int ny, int nz = 1);

    virtual void setGridCoords(int axisIdx, int size, const vec &full,
                               const vec &half);

    virtual void setGridCoords(int axisIdx, int size, const vec &full);

    vec getGridCoords(int axisIdx, int gridType,
                      bool hasVirtualGrids = false) const;

    double getGridCoordComp(int axisIdx, int gridType, int gridIdx) const;

    /**
     *  Get the space coordinate of a grid with given location.
     *
     *  @param i   the grid index (the cells are arranged as 1D array).
     *  @param loc the grid location.
     *  @param x   the grid space coordinate.
     */
    void getGridCoord(int i, int loc, CoordType &x) const;

    /**
     *  Get the space coordinate of a grid with given location.
     *
     *  @param i   the grid index (the cells are arranged as 1D array).
     *  @param loc the grid location.
     *  @return The grid space coordinate.
     */
    const CoordType& getGridCoord(int i, int loc) const;

    virtual void setCellVolumes();

    virtual double getCellVolume(int cellIdx) const;

    double getGridInterval(int axisIdx, int gridType, int gridIdx) const;

    virtual int getTotalNumGrid(int loc, int numDim) const;

    int getNumGrid(int axisIdx, int gridType,
                   bool hasVirtualGrids = false) const;

    virtual int getLevelIndex(int i, int loc) const;

    virtual void unwrapIndex(int cellIdx, int i[3], int loc) const;

    virtual int wrapIndex(int i, int j, int loc) const;

    virtual int wrapIndex(int i, int j, int k, int loc) const;

    virtual bool isHorizontalGridsSame(const Mesh<DomainType, CoordType> &other) const;
};

} // geomtk

#include "StructuredMesh-impl.h"

#endif // __Geomtk_StructuredMesh__
