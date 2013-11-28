#ifndef __Mesh__
#define __Mesh__

#include "commons.h"
#include "Domain.h"

enum StaggerType {
    CENTER, EDGE, VERTEX
};

// TODO: Use ArakawaGrid to set field conveniently.
enum ArakawaGrid {
	A_GRID, B_GRID, C_GRID, D_GRID, E_GRID
};

class Mesh {
protected:
    Domain *domain;
public:
    Mesh(Domain &domain);
    virtual ~Mesh();

    const Domain& getDomain() const;
};

#endif
