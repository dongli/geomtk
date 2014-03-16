#ifndef __RLLMesh__
#define __RLLMesh__

#include "StructuredMesh.h"

namespace geomtk {

struct RLLStagger : public StructuredStagger {
};

class SphereCoord;
class SphereVelocity;
class RLLMeshIndex;

class RLLMesh : public StructuredMesh {
public:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
    vec cosLonFull, sinLonFull;
    vec cosLonHalf, sinLonHalf;
    vec cosLatFull, sinLatFull, sinLatFull2;
    vec cosLatHalf, sinLatHalf, sinLatHalf2;
    vec tanLatFull, tanLatHalf;
    double poleRadius; // virtual effective radius of pole
public:
    RLLMesh(Domain &domain);
    virtual ~RLLMesh();

    void setPoleRadius(double radius);
    double getPoleRadius() const;

    virtual void setGridCoords(int axisIdx, int size, const vec &full,
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
};

}

#endif
