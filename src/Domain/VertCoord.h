#ifndef __Geomtk_VertCoord__
#define __Geomtk_VertCoord__

namespace geomtk {

enum VertCoordType {
    HEIGHT, PRESSURE, CLASSIC_PRESSURE_SIGMA, HYBRID_PRESSURE_SIGMA
};

/**
 *  This class is the interface for all the vertical coordinate.
 */
class VertCoord {
protected:
    VertCoordType _type;
public:
    VertCoord() {}
    virtual ~VertCoord() {}

    VertCoordType type() const { return _type; }
};

} // geomtk

#endif // __Geomtk_VertCoord__