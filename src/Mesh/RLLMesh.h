#ifndef __RLLMesh__
#define __RLLMesh__

#include "StructuredMesh.h"

namespace geomtk {

class SphereCoord;
class SphereVelocity;
class RLLMeshIndex;

class RLLMesh : public StructuredMesh {
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

    virtual void setGridCoords(int dim, int size, double *full, double *half);

    virtual void setCellVolumes();

    virtual void getGridCoord(const MeshIndex &idx, SpaceCoord &x,
                              ArakawaGrid gridType, int dim = 0) const;
    virtual void getGridCoord(int idx, SpaceCoord &x, ArakawaGrid gridType,
                              int dim = 0) const;

    double getCosLon(StaggerType staggerType, int i) const;
    double getSinLon(StaggerType staggerType, int i) const;
    double getCosLat(StaggerType staggerType, int j) const;
    double getSinLat(StaggerType staggerType, int j) const;
    double getSinLat2(StaggerType staggerType, int j) const;
    double getTanLat(StaggerType staggerType, int j) const;

    void move(const SphereCoord &x0, double dt, const SphereVelocity &v,
              const RLLMeshIndex &idx, SphereCoord &x1) const;
};

}

#endif
