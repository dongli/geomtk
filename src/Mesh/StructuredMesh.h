#ifndef __StructuredMesh__
#define __StructuredMesh__

#include "Mesh.h"

class StructuredMesh : public Mesh {
protected:
    vec *fullCoords;
    vec *halfCoords;
    vec *fullIntervals;
    vec *halfIntervals;
public:
    StructuredMesh(Domain &domain);
    virtual ~StructuredMesh();

    void setGridCoords(int dim, int size, double *full, double *half);
    const vec& getGridCoords(int dim, StaggerType staggerType) const;

    int getNumGrid(int dim, StaggerType staggerType) const;
};

#endif
