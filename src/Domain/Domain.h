#ifndef __Geomtk_Domain__
#define __Geomtk_Domain__

#include "geomtk_commons.h"

namespace geomtk {

enum BndType {
    PERIODIC, OPEN, POLE, RIGID, INVALID
};

/**
 *  This class describes the space coordinate of a point.
 */
class SpaceCoord {
protected:
    vec coord;
public:
    SpaceCoord(int numDim);
    SpaceCoord(const SpaceCoord &other);
    virtual ~SpaceCoord();

    virtual SpaceCoord& operator=(const SpaceCoord &other);

    virtual double operator()(int i) const;
    virtual double& operator()(int i);
    virtual const vec& operator()() const;
    virtual vec& operator()();

    virtual void print() const;
};

/**
 *  This class describes the body coordinate referenced to a point.
 */
class BodyCoord : public SpaceCoord {
public:
    BodyCoord(int numDim);
    virtual ~BodyCoord();
};

// -----------------------------------------------------------------------------

class Velocity {
protected:
    vec v;
public:
    Velocity();
    Velocity(int numDim);
    virtual ~Velocity();

    virtual void setNumDim(int numDim);

    double operator()(int i) const;
    double& operator()(int i);
    vec& operator()();

    const Velocity operator+(const Velocity &other) const;
    const Velocity operator-(const Velocity &other) const;
    const Velocity operator*(double scale) const;
    const Velocity operator/(double scale) const;

    virtual void print() const;
};

// -----------------------------------------------------------------------------

class Domain {
protected:
    int numDim;
    vector<string> axisName;
    vector<string> axisLongName;
    vector<string> axisUnits;
    vec axisStarts;
    vec axisEnds;
    vec axisSpans;
    BndType *bndTypeStarts;
    BndType *bndTypeEnds;
public:
    Domain();
    Domain(int numDim);
    virtual ~Domain();

    virtual int getNumDim() const;
    virtual void setAxis(int dim, const string &axisName,
                         const string &axisLongName, const string &axisUnits,
                         double start, BndType bndTypeStart,
                         double end, BndType bndTypeEnd);
    virtual const string& getAxisName(int dim) const;
    virtual const string& getAxisLongName(int dim) const;
    virtual const string& getAxisUnits(int dim) const;
    virtual double getAxisStart(int dim) const;
    virtual double getAxisEnd(int dim) const;
    virtual double getAxisSpan(int dim) const;
    virtual BndType getAxisStartBndType(int dim) const;
    virtual BndType getAxisEndBndType(int dim) const;

    /**
     *  Check the given space coordinate, especially when the boundary condition
     *  is periodic. The out-of-range coordinate should be put back.
     *
     *  @param x the space coordinate.
     */
    virtual void check(SpaceCoord &x) const;

    /**
     *  Calculate distance between two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    virtual double calcDistance(const SpaceCoord &x, const SpaceCoord &y) const;

    /**
     *  Difference two space coordinates.
     *
     *  @param x the first space coordinate.
     *  @param y the second space coordinate.
     *
     *  @return The distance.
     */
    virtual vec diffCoord(const SpaceCoord &x, const SpaceCoord &y) const;

    /**
     *  Get a brief about the domain.
     *
     *  @return The brief.
     */
    virtual string getBrief() const;
};

}

#endif
