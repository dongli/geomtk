#ifndef __Geomtk_BodyCoord__
#define __Geomtk_BodyCoord__

#include "SpaceCoord.h"

namespace geomtk {

/**
 *  This class describes the body coordinate referenced to a point.
 */
class BodyCoord : public SpaceCoord {
public:
    BodyCoord(int numDim) : SpaceCoord(numDim) {}
    virtual ~BodyCoord() {}
};

} // geomtk

#endif // __Geomtk_BodyCoord__