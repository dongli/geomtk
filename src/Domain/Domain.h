#ifndef __GEOMTK_Domain__
#define __GEOMTK_Domain__

#include "geomtk_commons.h"
#include "SpaceCoord.h"
#include "BodyCoord.h"
#include "VertCoord.h"

namespace geomtk {

enum BndType {
    PERIODIC, OPEN, POLE, RIGID, INVALID
};

static BndType
bndTypeFromString(const string &bndType) {
    if (bndType == "periodic") {
        return PERIODIC;
    } else if (bndType == "open") {
        return OPEN;
    } else if (bndType == "pole") {
        return POLE;
    } else if (bndType == "rigid") {
        return RIGID;
    } else {
        return INVALID;
    }
} // bndTypeFromString

/**
 *  This class describtes the Cartesian domain, and can be derived by other
 *  domains.
 */
template <typename CoordType>
class Domain {
protected:
    uword _numDim;
    vector<string> _axisName;
    vector<string> _axisLongName;
    vector<string> _axisUnits;
    vec _axisStarts;
    vec _axisEnds;
    vec _axisSpans;
    BndType *_bndTypeStarts;
    BndType *_bndTypeEnds;
    /**
     *  The vertical coordinate is so special that we need an object to handle
     *  it.
     */
    VertCoord *_vertCoord;
public:
    Domain();
    Domain(uword numDim);
    Domain(VertCoordType type);
    virtual ~Domain();

    virtual void
    init(const string &filePath) = 0;

    /**
     *  Return the dimension number of the domain.
     *
     *  @return The dimension number.
     */
    virtual uword
    numDim() const {
        return _numDim;
    }

    /**
     *  Set the axis along the specified dimension, including the meta info.
     *
     *  @param axisIdx      the axis index.
     *  @param axisName     the short name of the axis, e.g. 'x'.
     *  @param axisLongName the long name of the axis.
     *  @param axisUnits    the coordinate units of the axis.
     *  @param start        the start coordinate value.
     *  @param bndTypeStart the boundary type of the axis start.
     *  @param end          the end coordinate value.
     *  @param bndTypeEnd   the boundary type of the axis end.
     */
    virtual void
    setAxis(uword axisIdx, const string &axisName,
            const string &axisLongName, const string &axisUnits,
            double start, BndType bndTypeStart,
            double end, BndType bndTypeEnd);

    virtual const string&
    axisName(uword i) const {
        return _axisName[i];
    }

    virtual const string&
    axisLongName(uword i) const {
        return _axisLongName[i];
    }

    virtual const string&
    axisUnits(uword i) const {
        return _axisUnits[i];
    }

    virtual double
    axisStart(uword i) const {
        return _axisStarts(i);
    }

    virtual double
    axisEnd(uword i) const {
        return _axisEnds(i);
    }

    virtual double
    axisSpan(uword i) const {
        return _axisSpans(i);
    }

    virtual BndType
    axisStartBndType(uword i) const {
        return _bndTypeStarts[i];
    }

    virtual BndType
    axisEndBndType(uword i) const {
        return _bndTypeEnds[i];
    }

    VertCoord&
    vertCoord() {
        return *_vertCoord;
    }

    /**
     *  Check the given space coordinate, especially when the boundary condition
     *  is periodic. The out-of-range coordinate should be put back.
     *
     *  @param x the space coordinate.
     *
     *  @return Whether x is within the domain.
     */
    bool
    isValid(CoordType &x) const;

    bool
    isValid(const CoordType &x) const;

    /**
     *  Calculate distance between two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    virtual double
    calcDistance(const CoordType &x, const CoordType &y) const = 0;

    /**
     *  Difference two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    vec
    diffCoord(const SpaceCoord &x, const SpaceCoord &y) const;

    /**
     *  Get a brief about the domain.
     *
     *  @return The brief.
     */
    virtual string
    brief() const;
}; // Domain

} // geomtk

#include "Domain-impl.h"

#endif // __GEOMTK_Domain__
