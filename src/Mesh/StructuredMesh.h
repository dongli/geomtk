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

    void setCoords(int dim, int size, double *full, double *half);
};

#endif
