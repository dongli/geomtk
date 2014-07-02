#ifndef __Geomtk_StructuredMesh__
#define __Geomtk_StructuredMesh__

#include "Mesh.h"

namespace geomtk {

/**
 *  This class describes the grid stagger in structured mesh.
 */
struct StructuredStagger {
    struct GridType {
        static const int FULL = 0;
        static const int HALF = 1;
    };
    struct Location {
        static const int CENTER = 2;
        static const int X_FACE = 3;
        static const int Y_FACE = 4;
        static const int Z_FACE = 5;
        static const int XY_VERTEX = 6;
    };
};
    
class StructuredMeshIndex;

/**
 *  This class describes the structured mesh by storing the grid coordinates and
 *  providing several querying methods.
 */
class StructuredMesh : public Mesh {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
    field<double> volumes;
public:
    StructuredMesh(Domain &domain);
    virtual ~StructuredMesh();

    virtual void init(const string &fileName);

    virtual void init(int nx, int ny, int nz = 1);

    virtual void setGridCoords(int axisIdx, int size, const vec &full,
                               const vec &half);

    virtual void setGridCoords(int axisIdx, int size, const vec &full);

    vec getGridCoords(int axisIdx, int gridType,
                      bool hasVirtualGrids = false) const;

    double getGridCoordComp(int axisIdx, int gridType, int gridIdx) const;

    virtual void getGridCoord(int cellIdx, int loc, SpaceCoord &x) const;

    virtual void setCellVolumes();

    virtual double getCellVolume(int cellIdx) const;

    double getGridInterval(int axisIdx, int gridType, int gridIdx) const;

    virtual int getTotalNumGrid(int loc) const;

    int getNumGrid(int axisIdx, int gridType,
                   bool hasVirtualGrids = false) const;

    virtual void unwrapIndex(int cellIdx, int i[3], int loc) const;

    virtual int wrapIndex(const StructuredMeshIndex &meshIdx, int loc) const;
};

}

#endif
