#ifndef __GEOMTK__TerrainFollowing__
#define __GEOMTK__TerrainFollowing__

#include "VertCoord.h"

namespace geomtk {

/**
 *  This class is the interface for terrain following vertical coordinate.
 */
class TerrainFollowing : public VertCoord {
public:
    TerrainFollowing() {}
    virtual ~TerrainFollowing() {}

    virtual double calcPressure(double ps, int k) const = 0;
};

} // geomtk

#endif // __GEOMTK__TerrainFollowing__
