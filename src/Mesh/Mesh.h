#ifndef __Mesh__
#define __Mesh__

#include "commons.h"
#include "Domain.h"

enum StaggerType {
    CENTER, EDGE, VERTEX
};

class Mesh {
protected:
    Domain *domain;
public:
    Mesh(Domain &domain);
    virtual ~Mesh();
};

#endif
