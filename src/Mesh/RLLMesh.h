#ifndef __RLLMesh__
#define __RLLMesh__

#include "StructuredMesh.h"

class RLLMesh : public StructuredMesh {
protected:
    vec cosLonFull, sinLonFull;
    vec cosLonHalf, sinLonHalf;
    vec cosLatFull, sinLatFull, sinLatFull2;
    vec cosLatHalf, sinLatHalf, sinLatHalf2;
    double poleRadius; // virtual effective radius of pole
public:
    RLLMesh(Domain &domain);
    virtual ~RLLMesh();

    void setPoleRadius(double radius);
    double getPoleRadius() const;

    virtual void setGridCoords(int dim, int size, double *full, double *half);

    double getCosLon(StaggerType staggerType, int i);
    double getSinLon(StaggerType staggerType, int i);
    double getCosLat(StaggerType staggerType, int j);
    double getSinLat(StaggerType staggerType, int j);
    double getSinLat2(StaggerType staggerType, int j);
};

#endif