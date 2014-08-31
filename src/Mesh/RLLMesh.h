#ifndef __RLLMesh__
#define __RLLMesh__

#include "StructuredMesh.h"

namespace geomtk {

struct RLLStagger : public StructuredStagger {
};

class SphereDomain;
class SphereCoord;
class SphereVelocity;
class RLLMeshIndex;

class RLLMesh : public StructuredMesh<SphereDomain, SphereCoord> {
public:
    typedef SphereCoord CoordType;
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
private:
    vec cosLonFull, sinLonFull;
    vec cosLonHalf, sinLonHalf;
    vec cosLatFull, sinLatFull, sinLatFull2;
    vec cosLatHalf, sinLatHalf, sinLatHalf2;
    vec tanLatFull, tanLatHalf;
    double poleRadius; // virtual effective radius of pole
public:
    RLLMesh(SphereDomain &domain);
    virtual ~RLLMesh();

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

    void setPoleRadius(double radius) { poleRadius = radius; }

    double getPoleRadius() const { return poleRadius; }

    virtual void setGridCoordComps(int axisIdx, int size, const vec &full);

    virtual void setGridCoordComps(int axisIdx, int size, const vec &full,
                                   const vec &half);

    virtual void setCellVolumes();

    double getCosLon(int gridType, int i) const;

    double getSinLon(int gridType, int i) const;

    double getCosLat(int gridType, int j) const;

    double getSinLat(int gridType, int j) const;

    double getSinLat2(int gridType, int j) const;

    double getTanLat(int gridType, int j) const;

    void move(const SphereCoord &x0, double dt, const SphereVelocity &v,
              const RLLMeshIndex &idx, SphereCoord &x1) const;
protected:
    virtual void setGridCoords();
};

}

#endif
