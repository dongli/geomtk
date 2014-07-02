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
    Mesh(Domain &domain) { this->domain = &domain; set = false; }
    virtual ~Mesh() {}

    /**
     *  Read grids from file.
     *
     *  @param fileName the grid file name.
     */
    virtual void init(const string &fileName) = 0;

    /**
     *  Get the spatial domain.
     *
     *  @return The domain.
     */
    Domain& getDomain() const { return *domain; }

    virtual void getGridCoord(int cellIdx, int loc, SpaceCoord &x) const = 0;

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
    bool isSet() const { return set; }
};

}

#endif
