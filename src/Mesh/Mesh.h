#ifndef __Geomtk_Mesh__
#define __Geomtk_Mesh__

#include "geomtk_commons.h"
#include "Domain.h"

namespace geomtk {

class MeshIndex;

/**
 *  This is an abstract class.
 */

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
     *  Set the center grid cell volumes.
     */
    virtual void setCellVolumes() = 0;

    virtual double getCellVolume(int cellIdx) const = 0;
    
    virtual int getTotalNumGrid(int loc) const = 0;

    /**
     *  Check if the mesh is set or not.
     *
     *  @return The boolean flag.
     */
    bool isSet() const;
};

}

#endif
