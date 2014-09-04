#ifndef __Geomtk_Domain__
#define __Geomtk_Domain__

#include "geomtk_commons.h"
#include "SpaceCoord.h"
#include "BodyCoord.h"
#include "VertCoord.h"

namespace geomtk {

enum DomainType {
    CARTESIAN_DOMAIN, SPHERE_DOMAIN
};

enum BndType {
    PERIODIC, OPEN, POLE, RIGID, INVALID
};

/**
 *  This class describtes the Cartesian domain, and can be derived by other
 *  domains.
 */
template <typename CoordType>
class Domain {
protected:
    DomainType type;
    int numDim;
    vector<string> axisName;
    vector<string> axisLongName;
    vector<string> axisUnits;
    vec axisStarts;
    vec axisEnds;
    vec axisSpans;
    BndType *bndTypeStarts;
    BndType *bndTypeEnds;
    /**
     *  The vertical coordinate is so special that we need an object to handle
     *  it.
     */
    VertCoord *vertCoord;
public:
    Domain();
    Domain(int numDim);
    Domain(VertCoordType type);
    virtual ~Domain();

    /**
     *  Return the type of the domain.
     *
     *  @return The domain type enum.
     */
    DomainType getType() const { return type; }

    /**
     *  Return the dimension number of the domain.
     *
     *  @return The dimension number.
     */
    virtual int getNumDim() const { return numDim; }

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
    virtual void setAxis(int axisIdx, const string &axisName,
                         const string &axisLongName, const string &axisUnits,
                         double start, BndType bndTypeStart,
                         double end, BndType bndTypeEnd);

    /**
     *  Get the axis short name.
     *
     *  @param i the dimension index.
     *  @return The axis short name.
     */
    virtual const string& getAxisName(int i) const { return axisName[i]; }

    /**
     *  Get the axis long name.
     *
     *  @param i the dimension index.
     *  @return The axis long name.
     */
    virtual const string& getAxisLongName(int i) const { return axisLongName[i]; }

    /**
     *  Get the axis coordinate units.
     *
     *  @param i the dimension index.
     *  @return The axis coordinate units.
     */
    virtual const string& getAxisUnits(int i) const { return axisUnits[i]; }

    /**
     *  Get the axis start coordinate value.
     *
     *  @param i the dimension index.
     *  @return The axis start coordinate value.
     */
    virtual double getAxisStart(int i) const { return axisStarts(i); }

    /**
     *  Get the axis end coordinate value.
     *
     *  @param i the dimension index.
     *  @return The axis end coordinate value.
     */
    virtual double getAxisEnd(int i) const { return axisEnds(i); }

    /**
     *  Get the axis coordinate span.
     *
     *  @param i the dimension index.
     *  @return The axis coordinate span.
     */
    virtual double getAxisSpan(int i) const { return axisSpans(i); }

    /**
     *  Get the axis start boundary type.
     *
     *  @param i the dimension index.
     *  @return The axis start boundary type.
     */
    virtual BndType getAxisStartBndType(int i) const { return bndTypeStarts[i]; }

    /**
     *  Get the axis end boundary type.
     *
     *  @param i the dimension index.
     *  @return The axis end boundary type.
     */
    virtual BndType getAxisEndBndType(int i) const { return bndTypeEnds[i]; }

    /**
     *  Return the vertical coordinate object.
     *
     *  @return The vertical coordinate object.
     */
    VertCoord& getVertCoord() { return *vertCoord; }

    /**
     *  Constrain the given coordinate if the boundary condition is periodic.
     *
     *  @param x the space coordinate.
     */
    void constrain(CoordType &x) const;

    /**
     *  Check the given space coordinate, especially when the boundary condition
     *  is periodic. The out-of-range coordinate should be put back.
     *
     *  @param x the space coordinate.
     */
    bool isValid(const CoordType &x) const;

    /**
     *  Calculate distance between two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    virtual double calcDistance(const CoordType &x, const CoordType &y) const = 0;

    /**
     *  Difference two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    vec diffCoord(const SpaceCoord &x, const SpaceCoord &y) const;

    /**
     *  Get a brief about the domain.
     *
     *  @return The brief.
     */
    virtual string getBrief() const;
};

} // geomtk

#include "Domain-impl.h"

#endif // __Geomtk_Domain__
