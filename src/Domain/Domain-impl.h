#include "Domain.h"
#include "ClassicPressureSigma.h"
#include "HybridPressureSigma.h"

namespace geomtk {

template <typename CoordType>
Domain<CoordType>::Domain() {
    _type = CARTESIAN_DOMAIN;
    _numDim = 2;
    _axisName.resize(_numDim);
    _axisLongName.resize(_numDim);
    _axisUnits.resize(_numDim);
    _axisStarts.resize(_numDim);
    _axisEnds.resize(_numDim);
    _axisSpans.resize(_numDim);
    _bndTypeStarts = new BndType[_numDim];
    _bndTypeEnds = new BndType[_numDim];
    for (int i = 0; i < _numDim; ++i) {
        _bndTypeStarts[i] = INVALID;
        _bndTypeEnds[i] = INVALID;
    }
    _vertCoord = NULL;
}

template <typename CoordType>
Domain<CoordType>::Domain(int numDim) {
    _type = CARTESIAN_DOMAIN;
    this->_numDim = numDim;
    _axisName.resize(_numDim);
    _axisLongName.resize(_numDim);
    _axisUnits.resize(_numDim);
    _axisStarts.resize(_numDim);
    _axisEnds.resize(_numDim);
    _axisSpans.resize(_numDim);
    _bndTypeStarts = new BndType[_numDim];
    _bndTypeEnds = new BndType[_numDim];
    for (int i = 0; i < _numDim; ++i) {
        _bndTypeStarts[i] = INVALID;
        _bndTypeEnds[i] = INVALID;
    }
    _vertCoord = NULL;
}

template <typename CoordType>
Domain<CoordType>::Domain(VertCoordType vertCoordType) {
    this->_numDim = 3;
    _type = CARTESIAN_DOMAIN;
    _axisName.resize(_numDim);
    _axisLongName.resize(_numDim);
    _axisUnits.resize(_numDim);
    _axisStarts.resize(_numDim);
    _axisEnds.resize(_numDim);
    _axisSpans.resize(_numDim);
    _bndTypeStarts = new BndType[_numDim];
    _bndTypeEnds = new BndType[_numDim];
    for (int i = 0; i < _numDim; ++i) {
        _bndTypeStarts[i] = INVALID;
        _bndTypeEnds[i] = INVALID;
    }
    switch (vertCoordType) {
        case CLASSIC_PRESSURE_SIGMA:
            _vertCoord = new ClassicPressureSigma;
            break;
        case HYBRID_PRESSURE_SIGMA:
            _vertCoord = new HybridPressureSigma;
            break;
        default:
            _vertCoord = NULL;
            break;
    }
}

template <typename CoordType>
Domain<CoordType>::~Domain() {
    delete [] _bndTypeStarts;
    delete [] _bndTypeEnds;
    if (_vertCoord != NULL) {
        delete _vertCoord;
    }
}

template <typename CoordType>
void Domain<CoordType>::
setAxis(int dim, const string &name, const string &longName,
        const string &units, double start, BndType bndTypeStart,
        double end, BndType bndTypeEnd) {
    // sanity check
    if (dim >= _numDim) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds numDim (" << _numDim << ")!")
    }
    if (bndTypeStart == PERIODIC || bndTypeEnd == PERIODIC) {
        if (bndTypeStart != bndTypeEnd) {
            REPORT_ERROR("Argument bndTypeStart and bndTypeEnd should be PERIODIC at the same time!")
        }
    }
    if (start >= end) {
        REPORT_WARNING("Axis start is bigger than end!")
    }
    _axisName[dim] = name;
    _axisLongName[dim] = longName;
    _axisUnits[dim] = units;
    _axisStarts(dim) = start;
    _bndTypeStarts[dim] = bndTypeStart;
    _axisEnds(dim) = end;
    _bndTypeEnds[dim] = bndTypeEnd;
    _axisSpans(dim) = end-start;
}

template <typename CoordType>
string Domain<CoordType>::brief() const {
    static string brief = "normal domain";
    return brief;
}

template <typename CoordType>
void Domain<CoordType>::constrain(CoordType &x) const {
    for (int m = 0; m < _numDim; ++m) {
        if (x(m) < axisStart(m)) {
            if (axisStartBndType(m) == PERIODIC) {
                x(m) += axisSpan(m);
            } else {
                REPORT_ERROR("Coordinate is out of range!");
            }
        } else if (x(m) > axisEnd(m)) {
            if (axisStartBndType(m) == PERIODIC) {
                x(m) -= axisSpan(m);
            } else {
                REPORT_ERROR("Coordinate is out of range!");
            }
        }
    }
}

template <typename CoordType>
bool Domain<CoordType>::isValid(const CoordType &x) const {
    for (int m = 0; m < _numDim; ++m) {
        if (x(m) < axisStart(m) || x(m) > axisEnd(m)) {
            return false;
        }
    }
    return true;
}

} // geomtk
