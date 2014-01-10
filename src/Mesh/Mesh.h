#ifndef __Geomtk_Mesh__
#define __Geomtk_Mesh__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

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
    bool set;
public:
    Mesh(Domain &domain);
    virtual ~Mesh();

    const Domain& getDomain() const;
    bool isSet() const;
};

}

#endif
