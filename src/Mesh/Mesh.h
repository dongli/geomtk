#ifndef __Geomtk_Mesh__
#define __Geomtk_Mesh__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

enum StaggerType {
    CENTER, EDGE, VERTEX
};

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

    /**
     *  Get the spatial domain.
     *
     *  @return The domain.
     */
    const Domain& getDomain() const;

    /**
     *  Check if the mesh is set or not.
     *
     *  @return The boolean flag.
     */
    bool isSet() const;
};

}

#endif
