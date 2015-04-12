#ifndef __GEOMTK_BodyCoord__
#define __GEOMTK_BodyCoord__

#include "SpaceCoord.h"

namespace geomtk {

/**
 *  This class describes the body coordinate referenced to a point.
 */
class BodyCoord : public SpaceCoord {
public:
    BodyCoord() : SpaceCoord() {}
    BodyCoord(int numDim) : SpaceCoord(numDim) {}
    virtual ~BodyCoord() {}
}; // BodyCoord

} // geomtk

#endif // __GEOMTK_BodyCoord__