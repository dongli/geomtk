#include "Domain.h"

SpaceCoord::SpaceCoord(int numDim) {
    coords.resize(numDim);
}

SpaceCoord::~SpaceCoord() {
}

double SpaceCoord::operator()(int i) const {
    return coords(i);
}

double& SpaceCoord::operator()(int i) {
    return coords(i);
}

vec& SpaceCoord::operator()() {
    return coords;
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

double Velocity::operator()(int i) const {
    return v(i);
}

double& Velocity::operator()(int i) {
    return v(i);
}

vec& Velocity::operator()() {
    return v;
}

// -----------------------------------------------------------------------------

Domain::Domain() {
    this->numDim = 2;
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

Domain::Domain(int numDim) {
    this->numDim = numDim;
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

int Domain::getNumDim() const {
    return numDim;
}

void Domain::setAxis(int dim,
                     double start, BndType bndTypeStart,
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

    axisStarts(dim) = start;
    bndTypeStarts[dim] = bndTypeStart;
    axisEnds(dim) = end;
    bndTypeEnds[dim] = bndTypeEnd;
    axisSpans(dim) = end-start;
}

double Domain::getAxisStart(int dim) const {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisStarts(dim);
}

double Domain::getAxisEnd(int dim) const {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisEnds(dim);
}

double Domain::getAxisSpan(int dim) const {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisSpans(dim);
}

BndType Domain::getAxisStartBndType(int dim) const {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return bndTypeStarts[dim];
}

BndType Domain::getAxisEndBndType(int dim) const {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return bndTypeEnds[dim];
}

double Domain::calcDistance(const SpaceCoord &x, const SpaceCoord &y) const {
    REPORT_ERROR("Domain does not implement this method!");
}
