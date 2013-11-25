#include "Domain.h"

Domain::Domain(int numDim) {
    this->numDim = numDim;
    axisStarts.resize(numDim);
    axisEnds.resize(numDim);
    axisSpans.resize(numDim);
    bndTypeStarts = new BndType[numDim];
    bndTypeEnds = new BndType[numDim];
}

Domain::~Domain() {
    delete [] bndTypeStarts;
    delete [] bndTypeEnds;
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
        REPORT_ERROR("Axis start is smaller than end!")
    }

    axisStarts(dim) = start;
    bndTypeStarts[dim] = bndTypeStart;
    axisEnds(dim) = end;
    bndTypeEnds[dim] = bndTypeEnd;
    axisSpans(dim) = end-start;
}

double Domain::getAxisStart(int dim) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisStarts(dim);
}

double Domain::getAxisEnd(int dim) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisEnds(dim);
}

double Domain::getAxisSpan(int dim) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return axisSpans(dim);
}

BndType Domain::getAxisStartBndType(int dim) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return bndTypeStarts[dim];
}

BndType Domain::getAxisEndBndType(int dim) {
    // sanity check
    if (dim >= numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << numDim << ")!")
    }
    return bndTypeEnds[dim];
}
