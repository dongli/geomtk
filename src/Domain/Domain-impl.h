#include "Domain.h"
#include "ClassicPressureSigma.h"
#include "HybridPressureSigma.h"

namespace geomtk {

template <typename CoordType>
Domain<CoordType>::Domain() {
    type = CARTESIAN_DOMAIN;
    numDim = 2;
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
    vertCoord = NULL;
}

template <typename CoordType>
Domain<CoordType>::Domain(int numDim) {
    type = CARTESIAN_DOMAIN;
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
    vertCoord = NULL;
}

template <typename CoordType>
Domain<CoordType>::Domain(VertCoordType vertCoordType) {
    this->numDim = 3;
    type = CARTESIAN_DOMAIN;
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
    switch (vertCoordType) {
        case CLASSIC_PRESSURE_SIGMA:
            vertCoord = new ClassicPressureSigma;
            break;
        case HYBRID_PRESSURE_SIGMA:
            vertCoord = new HybridPressureSigma;
            break;
        default:
            vertCoord = NULL;
            break;
    }
}

template <typename CoordType>
Domain<CoordType>::~Domain() {
    delete [] bndTypeStarts;
    delete [] bndTypeEnds;
    if (vertCoord != NULL) {
        delete vertCoord;
    }
}

template <typename CoordType>
void Domain<CoordType>::setAxis(int dim, const string &name, const string &longName,
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
        REPORT_WARNING("Axis start is bigger than end!")
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

template <typename CoordType>
string Domain<CoordType>::getBrief() const {
    static string brief = "normal domain";
    return brief;
}

template <typename CoordType>
void Domain<CoordType>::constrain(CoordType &x) const {
    for (int m = 0; m < numDim; ++m) {
        if (x(m) < getAxisStart(m)) {
            if (getAxisStartBndType(m) == PERIODIC) {
                x(m) += getAxisSpan(m);
            } else {
                REPORT_ERROR("Coordinate is out of range!");
            }
        } else if (x(m) > getAxisEnd(m)) {
            if (getAxisStartBndType(m) == PERIODIC) {
                x(m) -= getAxisSpan(m);
            } else {
                REPORT_ERROR("Coordinate is out of range!");
            }
        }
    }
}

template <typename CoordType>
bool Domain<CoordType>::isValid(const CoordType &x) const {
    for (int m = 0; m < numDim; ++m) {
        if (x(m) < getAxisStart(m) || x(m) > getAxisEnd(m)) {
            return false;
        }
    }
    return true;
}

} // geomtk
