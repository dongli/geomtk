#include "Domain.h"

namespace geomtk {

SpaceCoord::SpaceCoord(int numDim) {
    coord.resize(numDim);
}

SpaceCoord::SpaceCoord(const SpaceCoord &other) {
    coord.resize(other.coord.size());
    *this = other;
}

SpaceCoord::~SpaceCoord() {
}

void SpaceCoord::print() const {
    cout << "Coordinate:";
    for (int i = 0; i < coord.size(); ++i) {
        cout << setw(20) << setprecision(10) << coord(i);
    }
    cout << endl;
}

// -----------------------------------------------------------------------------

BodyCoord::BodyCoord(int numDim) : SpaceCoord(numDim) {
}
    
BodyCoord::~BodyCoord() {
}

// -----------------------------------------------------------------------------

Velocity::Velocity() {
    v.resize(3);
}

Velocity::Velocity(int numDim) {
    v.resize(numDim);
}

Velocity::~Velocity() {
}

void Velocity::setNumDim(int numDim) {
    v.resize(numDim);
}

void Velocity::print() const {
    cout << "Velocity:";
    for (int i = 0; i < v.size(); ++i) {
        cout << setw(20) << v(i);
    }
    cout << endl;
}

// -----------------------------------------------------------------------------

Domain::Domain() : Domain(2) {
}

Domain::Domain(int numDim) {
    this->numDim = numDim;
    axisName.resize(numDim);
    axisLongName.resize(numDim);
    axisUnits.resize(numDim);
    axisStarts.resize(numDim);
    axisEnds.resize(numDim);
    axisSpans.resize(numDim);
    bndTypeStarts = new BndType[numDim];
    bndTypeEnds = new BndType[numDim];
    for (int i = 0; i < numDim; ++i) {
        bndTypeStarts[i] = INVALID;
        bndTypeEnds[i] = INVALID;
    }
}

Domain::~Domain() {
    delete [] bndTypeStarts;
    delete [] bndTypeEnds;
}

void Domain::setAxis(int dim, const string &name, const string &longName,
                     const string &units, double start, BndType bndTypeStart,
                     double end, BndType bndTypeEnd) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    if (bndTypeStart == PERIODIC || bndTypeEnd == PERIODIC) {
        if (bndTypeStart != bndTypeEnd) {
            REPORT_ERROR("Argument bndTypeStart and bndTypeEnd should be PERIODIC at the same time!")
        }
    }
    if (start >= end) {
        REPORT_WARNING("Axis start is smaller than end!")
    }
    axisName[dim] = name;
    axisLongName[dim] = longName;
    axisUnits[dim] = units;
    axisStarts(dim) = start;
    bndTypeStarts[dim] = bndTypeStart;
    axisEnds(dim) = end;
    bndTypeEnds[dim] = bndTypeEnd;
    axisSpans(dim) = end-start;
}

void Domain::check(geomtk::SpaceCoord &x) const {
    for (int m = 0; m < numDim; ++m) {
        if (x(m) < getAxisStart(m)) {
            if (getAxisStartBndType(m) == PERIODIC) {
                x(m) += getAxisSpan(m);
            } else {
                x.print();
                REPORT_ERROR("Space coordinate is out of range!");
            }
        } else if (x(m) > getAxisEnd(m)) {
            if (getAxisStartBndType(m) == PERIODIC) {
                x(m) -= getAxisSpan(m);
            } else {
                x.print();
                REPORT_ERROR("Space coordinate is out of range!");
            }
        }
    }
}
    
double Domain::calcDistance(const SpaceCoord &x, const SpaceCoord &y) const {
    REPORT_ERROR("Domain does not implement this method!");
}
    
vec Domain::diffCoord(const SpaceCoord &x, const SpaceCoord &y) const {
    return x()-y();
}

string Domain::getBrief() const {
    static string brief = "normal domain";
    return brief;
}

}
