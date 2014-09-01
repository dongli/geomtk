#include "Domain.h"
#include "ClassicPressureSigma.h"
#include "HybridPressureSigma.h"

namespace geomtk {

Domain::Domain() {
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

Domain::Domain(int numDim) {
    assert(numDim < 3);
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

Domain::Domain(VertCoordType vertCoordType) {
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

Domain::~Domain() {
    delete [] bndTypeStarts;
    delete [] bndTypeEnds;
    if (vertCoord != NULL) {
        delete vertCoord;
    }
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
