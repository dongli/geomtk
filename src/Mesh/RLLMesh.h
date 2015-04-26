#ifndef __GEOMTK_RLLMesh__
#define __GEOMTK_RLLMesh__

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
    double _poleRadius; // virtual effective radius of pole
public:
    RLLMesh(SphereDomain &domain);
    virtual ~RLLMesh();

    virtual void
    init(const string &filePath);

    virtual void
    init(const string &filePathH, const string &filePathV);

    virtual void
    init(uword nx, uword ny, uword nz = 1);

    double&
    poleRadius() {
        return _poleRadius;
    }

    double
    poleRadius() const {
        return _poleRadius;
    }

    virtual void
    setGridCoordComps(uword axisIdx, uword size, const vec &full);

    virtual void
    setGridCoordComps(uword axisIdx, uword size,
                      const vec &full, const vec &half);

    virtual void
    setCellVolumes();

    double
    cosLon(int gridType, int i) const;

    double
    sinLon(int gridType, int i) const;

    double
    cosLat(int gridType, int j) const;

    double
    sinLat(int gridType, int j) const;

    double
    sinLat2(int gridType, int j) const;

    double
    tanLat(int gridType, int j) const;

    void
    move(const SphereCoord &x0, double dt, const SphereVelocity &v,
         const RLLMeshIndex &idx, SphereCoord &x1) const;
protected:
    virtual void
    setGridCoords();
}; // RLLMesh

} // geomtk

#endif // __GEOMTK_RLLMesh__
