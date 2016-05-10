#ifndef __GEOMTK_VertCoord__
#define __GEOMTK_VertCoord__

namespace geomtk {

enum VertCoordType {
    HEIGHT, PRESSURE, CLASSIC_PRESSURE_SIGMA, HYBRID_PRESSURE_SIGMA
};

struct VertCoordStagger {
    struct Location {
        static const int CENTER = 0;
        static const int VERTEX = 1;
    };
    struct GridType {
        static const int FULL = 0;
        static const int HALF = 1;
    };
};

/**
 *  This class is the interface for all the vertical coordinate.
 */
class VertCoord {
public:
    typedef VertCoordStagger::GridType GridType;
    typedef VertCoordStagger::Location Location;
protected:
    VertCoordType _type;
public:
    VertCoord() {}
    virtual ~VertCoord() {}

    VertCoordType
    type() const {
        return _type;
    }
}; // VertCoord

} // geomtk

#endif // __GEOMTK_VertCoord__
