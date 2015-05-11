namespace geomtk {

template <class DomainType, class CoordType>
StructuredMesh<DomainType, CoordType>::
StructuredMesh(DomainType &domain, uword haloWidth)
: Mesh<DomainType, CoordType>(domain) {
    _haloWidth = haloWidth;
    fullIndexRanges.set_size(2, 3);
    halfIndexRanges.set_size(2, 3);
    fullIndexRanges.fill(0);
    halfIndexRanges.fill(0);
    fullCoords = new vec[domain.numDim()];
    halfCoords = new vec[domain.numDim()];
    fullIntervals = new vec[domain.numDim()];
    halfIntervals = new vec[domain.numDim()];
}

template <class DomainType, class CoordType>
StructuredMesh<DomainType, CoordType>::
~StructuredMesh() {
    delete [] fullCoords;
    delete [] halfCoords;
    delete [] fullIntervals;
    delete [] halfIntervals;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
init(const string &filePath) {
    setGridTypes();
    setCellVolumes();
    setGridCoords();
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
init(const string &filePathH, const string &filePathV) {
    setGridTypes();
    setCellVolumes();
    setGridCoords();
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
init(uword nx, uword ny, uword nz) {
    vec full, half;
    uword n[3] = {nx, ny, nz};
    double dx;
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if (this->domain().axisStartBndType(m) == PERIODIC) {
            full.resize(n[m]);
            half.resize(n[m]);
            dx = this->domain().axisSpan(m)/n[m];
            for (uword i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+i*dx;
                half[i] = full[i]+0.5*dx;
            }
        } else if (this->domain().axisStartBndType(m) == POLE &&
                   this->domain().axisEndBndType(m) == POLE) {
            full.resize(n[m]);
            half.resize(n[m]-1);
            dx = this->domain().axisSpan(m)/(n[m]-1);
            for (uword i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+i*dx;
            }
            for (uword i = 0; i < n[m]-1; ++i) {
                half[i] = full[i]+0.5*dx;
            }
        } else if ((this->domain().axisStartBndType(m) == RIGID &&
                    this->domain().axisEndBndType(m) == RIGID) ||
                   (this->domain().axisStartBndType(m) == OPEN &&
                    this->domain().axisEndBndType(m) == OPEN)) {
            full.resize(n[m]);
            half.resize(n[m]+1);
            dx = this->domain().axisSpan(m)/n[m];
            for (uword i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+0.5*dx+i*dx;
            }
            for (uword i = 0; i < n[m]+1; ++i) {
                half[i] = this->domain().axisStart(m)+i*dx;
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        setGridCoordComps(m, n[m], full, half);
    }
    this->set = true;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
setGridCoordComps(uword axisIdx, uword size, const vec &full, const vec &half) {
    // sanity check
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC) {
        if (full.size() != half.size()) {
            REPORT_ERROR("Full grid size (" << full.size() << ") should " <<
                         "be equal with half grid (" << half.size() << ")!");
        }
        fullCoords[axisIdx].set_size(size+2*_haloWidth);
        halfCoords[axisIdx].set_size(size+2*_haloWidth);
        fullIntervals[axisIdx].set_size(size+2*_haloWidth-1);
        halfIntervals[axisIdx].set_size(size+2*_haloWidth-1);
        // Set halo grids.
        for (uword i = 0; i < _haloWidth; ++i) {
            fullCoords[axisIdx](i) = full[size-_haloWidth+i]-this->domain().axisSpan(axisIdx);
            fullCoords[axisIdx](size+_haloWidth+i) = full[i]+this->domain().axisSpan(axisIdx);
            halfCoords[axisIdx](i) = half[size-_haloWidth+i]-this->domain().axisSpan(axisIdx);
            halfCoords[axisIdx](size+_haloWidth+i) = half[i]+this->domain().axisSpan(axisIdx);
        }
        // Set real grids.
        for (uword i = 0; i < size; ++i) {
            fullCoords[axisIdx](i+_haloWidth) = full[i];
            halfCoords[axisIdx](i+_haloWidth) = half[i];
        }
        // Set grid intervals.
        for (uword i = 0; i < fullCoords[axisIdx].size()-1; ++i) {
            fullIntervals[axisIdx](i) = fullCoords[axisIdx](i+1)-fullCoords[axisIdx](i);
        }
        for (uword i = 0; i < halfCoords[axisIdx].size()-1; ++i) {
            halfIntervals[axisIdx](i) = halfCoords[axisIdx](i+1)-halfCoords[axisIdx](i);
        }
        if (fabs(full[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            /*
             o - full real grid   0 - full halo grid
             * - half real grid   x - half halo grid
             
             size = 4, halo width = 1
              _______________________________________
             |                                       |
             |    0         1         2         3    |    4         5
             0         1         2         3         4         5
             0----x----o----*----o----*----o----*----o----*----0----x
                       |                                       |
                       |_______________________________________|
                     start                                    end
             -----------
            fullInterval
                  -----------
                 halfInterval
             */
            // Note: Make sure this first full grid coordinate is equal with the
            // axis starting coordinate to ease the following judging.
            fullCoords[axisIdx](_haloWidth) = this->domain().axisStart(axisIdx);
        } else if (fabs(half[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            /*
             o - full real grid   0 - full halo grid
             * - half real grid   x - half halo grid
             
             size = 4, halo width = 1
              _______________________________________
             |                                       |
             |    0         1         2         3    |    4         5
             0         1         2         3         4         5
             x----0----*----o----*----o----*----o----*----o----x----0
                       |                                       |
                       |_______________________________________|
                     start                                    end
             -----------
            fullInterval
                  -----------
                 halfInterval
             */
            // Note: Make sure this first half grid coordinate is equal with the
            // axis starting coordinate to ease the following judging.
            halfCoords[axisIdx](_haloWidth) = this->domain().axisStart(axisIdx);
        } else {
            REPORT_ERROR("Don't know how to handle input grid coordinates "
                         "of dimension " << axisIdx << "!");
        }
    } else if (this->domain().axisStartBndType(axisIdx) == POLE ||
               this->domain().axisStartBndType(axisIdx) == RIGID ||
               this->domain().axisStartBndType(axisIdx) == OPEN) {
        if (fabs(full[0]-this->domain().axisStart(axisIdx)) < 1.0e-14 &&
            fabs(full[size-1]-this->domain().axisEnd(axisIdx)) < 1.0e-14) {
            /*
                o - full real grid
                * - half real grid
             
                size = 6
             
                     0         1         2         3         4
                0         1         2         3         4         5
                o----*----o----*----o----*----o----*----o----*----o
                |                                                 |
              start                                              end
                -----------
               fullInterval
                     -----------
                    halfInterval
                -----
                placeholder halfInterval
             */
            fullCoords[axisIdx].set_size(size);
            halfCoords[axisIdx].set_size(size-1);
            fullIntervals[axisIdx].set_size(size-1);
            halfIntervals[axisIdx].set_size(size);
            for (uword i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (uword i = 0; i < size-1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // Note: Make sure the both ends of the full grid coordinates are
            // equal with the axis starting and ending coordiante to ease the
            // following judging.
            fullCoords[axisIdx](0) = this->domain().axisStart(axisIdx);
            fullCoords[axisIdx](size-1) = this->domain().axisEnd(axisIdx);
            for (uword i = 0; i < size-1; ++i) {
                fullIntervals[axisIdx](i) = full[i+1]-full[i];
            }
            for (uword i = 1; i < size-1; ++i) {
                halfIntervals[axisIdx](i) = half[i]-half[i-1];
            }
            // Set placeholder half intervals.
            halfIntervals[axisIdx](0) = half[0]-this->domain().axisStart(axisIdx);
            halfIntervals[axisIdx](size-1) = this->domain().axisEnd(axisIdx)-half[size-2];
        } else if (fabs(half[0]-this->domain().axisStart(axisIdx)) < 1.0e-14 &&
                   fabs(half[size]-this->domain().axisEnd(axisIdx)) < 1.0e-14) {
            /*
                o - full grid
                * - half grid
             
                size = 5
             
                     0         1         2         3         4
                0         1         2         3         4         5
                *----o----*----o----*----o----*----o----*----o----*
                |                                                 |
              start                                              end
                -----------
               halfInterval
                     -----------
                    fullInterval
                -----
                placeholder fullInterval
             */
            fullCoords[axisIdx].set_size(size);
            halfCoords[axisIdx].set_size(size+1);
            fullIntervals[axisIdx].set_size(size+1);
            halfIntervals[axisIdx].set_size(size);
            for (uword i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (uword i = 0; i < size+1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // Note: Make sure the both ends of the half grid coordinates are
            // equal with the axis starting and ending coordiante to ease the
            // following judging.
            halfCoords[axisIdx](0) = this->domain().axisStart(axisIdx);
            halfCoords[axisIdx](size) = this->domain().axisEnd(axisIdx);
            for (uword i = 1; i < size; ++i) {
                fullIntervals[axisIdx](i) = full[i]-full[i-1];
            }
            fullIntervals[axisIdx](0) = full[0]-this->domain().axisStart(axisIdx);
            fullIntervals[axisIdx](size) = this->domain().axisEnd(axisIdx)-full[size-1];
            for (uword i = 0; i < size; ++i) {
                halfIntervals[axisIdx](i) = half[i+1]-half[i];
            }
        } else {
            REPORT_ERROR("Unhandled branch!");
        }
    } else if (this->domain().axisStartBndType(axisIdx) == INVALID) {
        REPORT_ERROR("Axis " << axisIdx << " is not set!");
    } else {
        REPORT_ERROR("Unhandled branch!");
    }
    // Check if all the grid components are set.
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if (fullCoords[m].size() == 0) {
            return;
        }
    }
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        // Set the default grid index ranges.
        if (this->domain().axisStartBndType(m) == PERIODIC) {
            fullIndexRanges(0, m) = _haloWidth;
            fullIndexRanges(1, m) = fullCoords[m].size()-_haloWidth-1;
            halfIndexRanges(0, m) = _haloWidth;
            halfIndexRanges(1, m) = halfCoords[m].size()-_haloWidth-1;
        } else {
            fullIndexRanges(0, m) = 0;
            fullIndexRanges(1, m) = fullCoords[m].size()-1;
            halfIndexRanges(0, m) = 0;
            halfIndexRanges(1, m) = halfCoords[m].size()-1;
        }
        // Set grid styles.
        if (fullCoords[m](startIndex(m, GridType::FULL)) ==
            this->domain().axisStart(m)) {
            gridStyles[m] = FULL_LEAD;
        } else if (halfCoords[m](startIndex(m, GridType::HALF)) ==
                   this->domain().axisStart(m)) {
            gridStyles[m] = HALF_LEAD;
        } else {
            REPORT_ERROR("Unexpected branch!");
        }
    }
    // Set other parameters.
    setGridTypes();
    setCellVolumes();
    setGridCoords();
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
is(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(0, 0);
        case GridType::HALF:
            return halfIndexRanges(0, 0);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
ie(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(1, 0);
        case GridType::HALF:
            return halfIndexRanges(1, 0);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
js(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(0, 1);
        case GridType::HALF:
            return halfIndexRanges(0, 1);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
je(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(1, 1);
        case GridType::HALF:
            return halfIndexRanges(1, 1);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
ks(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(0, 2);
        case GridType::HALF:
            return halfIndexRanges(0, 2);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
ke(int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(1, 2);
        case GridType::HALF:
            return halfIndexRanges(1, 2);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
startIndex(uword axisIdx, int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(0, axisIdx);
        case GridType::HALF:
            return halfIndexRanges(0, axisIdx);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
endIndex(uword axisIdx, int gridType) const {
    switch (gridType) {
        case GridType::FULL:
            return fullIndexRanges(1, axisIdx);
        case GridType::HALF:
            return halfIndexRanges(1, axisIdx);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
levelIndex(int cellIdx, int loc) const {
    uword i, j, k;
    unwrapIndex(loc, cellIdx, i, j, k);
    return k;
}

template <class DomainType, class CoordType>
bool StructuredMesh<DomainType, CoordType>::
isHorizontalGridsSame(const Mesh<DomainType, CoordType> &_other) const {
    if (this == &_other) {
        return true;
    }
    if (this->type() != _other.type()) {
        return false;
    }
    const StructuredMesh<DomainType, CoordType> &other = static_cast<const StructuredMesh<DomainType, CoordType>&>(_other);
    const double eps = 1.0e-14;
    for (uword m = 0; m < 2; ++m) {
        if (this->fullCoords[m].size() != other.fullCoords[m].size()) {
            return false;
        }
        for (uword i = 0; i < this->fullCoords[m].size(); ++i) {
            if (fabs(this->fullCoords[m][i]-other.fullCoords[m][i]) > eps) {
                return false;
            }
        }
        if (this->halfCoords[m].size() != other.halfCoords[m].size()) {
            return false;
        }
        for (uword i = 0; i < this->halfCoords[m].size(); ++i) {
            if (fabs(this->halfCoords[m][i]-other.halfCoords[m][i]) > eps) {
                return false;
            }
        }
    }
    return true;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
setGridCoordComps(uword axisIdx, uword size, const vec &full) {
    // sanity check
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    // construct the half grid coordinates from full grids
    vec half;
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC) {
        half.set_size(size);
        if (full[0] == this->domain().axisStart(axisIdx)) {
            for (uword i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
            half[size-1] = (full[size-1]+this->domain().axisEnd(axisIdx))*0.5;
        } else if (full[size-1] == this->domain().axisEnd(axisIdx)) {
            half[0] = (this->domain().axisStart(axisIdx)+full[0])*0.5;
            for (uword i = 1; i < size; ++i) {
                half[i] = (full[i-1]+full[i])*0.5;
            }
        }
    } else if (this->domain().axisStartBndType(axisIdx) == POLE ||
               this->domain().axisStartBndType(axisIdx) == RIGID ||
               this->domain().axisStartBndType(axisIdx) == OPEN) {
        if (fabs(full[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            assert(fabs(full[size-1]-this->domain().axisEnd(axisIdx)) < 1.0e-14);
            half.set_size(size-1);
            for (uword i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
        } else {
            half.set_size(size+1);
            half[0] = this->domain().axisStart(axisIdx);
            for (uword i = 1; i < size; ++i) {
                half[i] = (full[i]+full[i-1])*0.5;
            }
            half[size] = this->domain().axisEnd(axisIdx);
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
    setGridCoordComps(axisIdx, size, full, half);
}

template <class DomainType, class CoordType>
vec StructuredMesh<DomainType, CoordType>::
gridCoordComps(uword axisIdx, int gridType, bool hasHaloGrids) const {
#ifndef NDEBUG
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
#endif
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC && !hasHaloGrids) {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx](span(_haloWidth, fullCoords[axisIdx].size()-_haloWidth-1));
            case GridType::HALF:
                return halfCoords[axisIdx](span(_haloWidth, halfCoords[axisIdx].size()-_haloWidth-1));
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    } else {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx];
            case GridType::HALF:
                return halfCoords[axisIdx];
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    }
}

template <class DomainType, class CoordType>
double StructuredMesh<DomainType, CoordType>::
gridCoordComp(uword axisIdx, int gridType, int gridIdx) const {
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    switch (gridType) {
        case GridType::FULL:
            return fullCoords[axisIdx](gridIdx);
        case GridType::HALF:
            return halfCoords[axisIdx](gridIdx);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
const CoordType& StructuredMesh<DomainType, CoordType>::
gridCoord(int loc, int i) const {
    if (this->domain().numDim() != 1) {
        return gridCoords[loc][i];
    } else {
        return gridCoords[loc][wrapIndex(loc, i)];
    }
}

template <class DomainType, class CoordType>
const CoordType& StructuredMesh<DomainType, CoordType>::
gridCoord(int loc, int i, int j, int k) const {
    uword cellIdx;
    switch (this->domain().numDim()) {
        case 1:
            cellIdx = wrapIndex(loc, i);
            break;
        case 2:
            cellIdx = wrapIndex(loc, i, j);
            break;
        case 3:
            cellIdx = wrapIndex(loc, i, j, k);
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
    return gridCoords[loc][cellIdx];
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
dualGridLocation(int loc) const {
    switch (this->domain().numDim()) {
        case 1:
            switch (loc) {
                case Location::CENTER:
                    return Location::VERTEX;
                case Location::VERTEX:
                    return Location::CENTER;
                default:
                    REPORT_ERROR("Invalid grid location!");
            }
            break;
        case 2:
            switch (loc) {
                case Location::CENTER:
                    return Location::VERTEX;
                case Location::VERTEX:
                case Location::XY_VERTEX:
                    return Location::CENTER;
                case Location::X_FACE:
                    return Location::Y_FACE;
                case Location::Y_FACE:
                    return Location::X_FACE;
                default:
                    REPORT_ERROR("Invalid grid location!");
            }
        case 3:
            switch (loc) {
                case Location::CENTER:
                    return Location::VERTEX;
                case Location::VERTEX:
                    return Location::CENTER;
                case Location::X_FACE:
                    return Location::YZ_VERTEX;
                case Location::Y_FACE:
                    return Location::XZ_VERTEX;
                case Location::Z_FACE:
                    return Location::XY_VERTEX;
                case Location::XY_VERTEX:
                    return Location::Z_FACE;
                case Location::XZ_VERTEX:
                    return Location::Y_FACE;
                case Location::YZ_VERTEX:
                    return Location::X_FACE;
                default:
                    REPORT_ERROR("Invalid grid location!");
            }
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
}

template <class DomainType, class CoordType>
vec StructuredMesh<DomainType, CoordType>::
cellSize(int loc, int cellIdx) const {
    vec res(this->domain().numDim());
    uvec spanIdx = unwrapIndex(loc, cellIdx);
    for (uword m = 0; m < this->domain().numDim(); ++m) {
        if ((gridTypes(0, m, loc) == GridType::HALF && gridStyles[m] == FULL_LEAD) ||
            (gridTypes(0, m, loc) == GridType::FULL && gridStyles[m] == HALF_LEAD)) {
            res[m] = gridInterval(m, gridTypes[m], spanIdx[m]-1);
        } else {
            res[m] = gridInterval(m, gridTypes[m], spanIdx[m]);
        }
    }
    return res;
}

template <class DomainType, class CoordType>
double StructuredMesh<DomainType, CoordType>::
gridInterval(uword axisIdx, int gridType, int gridIdx) const {
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    // TODO: Check this out!
    switch (gridType) {
        case GridType::FULL:
            return fullIntervals[axisIdx](gridIdx);
        case GridType::HALF:
            return halfIntervals[axisIdx](gridIdx);
        default:
            REPORT_ERROR("Unknown grid type!");
    }
}

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
totalNumGrid(int loc, uword numDim) const {
    if (numDim == 0) {
        numDim = this->domain().numDim();
    }
#ifndef NDEBUG
    assert(numDim >= 1 && numDim <= 3);
#endif
    uword res = 1;
    for (uword m = 0; m < numDim; ++m) {
        res *= numGrid(m, gridTypes(0, m, loc));
    }
    return res;
} // totalNumGrid

template <class DomainType, class CoordType>
uword StructuredMesh<DomainType, CoordType>::
numGrid(uword axisIdx, int gridType, bool hasHaloGrids) const {
    if (axisIdx >= this->domain().numDim()) {
        return 1;
    }
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC && !hasHaloGrids) {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx].size()-2*_haloWidth;
            case GridType::HALF:
                return halfCoords[axisIdx].size()-2*_haloWidth;
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    } else {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx].size();
            case GridType::HALF:
                return halfCoords[axisIdx].size();
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    }
} // numGrid

template <class DomainType, class CoordType>
uvec StructuredMesh<DomainType, CoordType>::
unwrapIndex(int loc, int cellIdx) const {
    uvec res(this->domain().numDim());
    switch (this->domain().numDim()) {
        case 1:
            unwrapIndex(loc, cellIdx, res[0]);
            break;
        case 2:
            unwrapIndex(loc, cellIdx, res[0], res[1]);
            break;
        case 3:
            unwrapIndex(loc, cellIdx, res[0], res[1], res[2]);
            break;
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
    return res;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
unwrapIndex(int loc, int cellIdx, uword &i) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 1);
#endif
    auto gridType = gridTypes(0, 0, loc);
    i = cellIdx+startIndex(0, gridType);
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
unwrapIndex(int loc, int cellIdx, uword &i, uword &j) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 2);
#endif
    auto gridType = gridTypes(0, 0, loc);
    auto nx = numGrid(0, gridType);
    j = cellIdx/nx+startIndex(1, gridType);
    i = cellIdx%nx+startIndex(0, gridType);
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
unwrapIndex(int loc, int cellIdx, uword &i, uword &j, uword &k) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 3);
#endif
    auto gridTypeX = gridTypes(0, 0, loc);
    auto gridTypeY = gridTypes(0, 1, loc);
    auto gridTypeZ = gridTypes(0, 2, loc);
    auto nx = numGrid(0, gridTypeX);
    auto ny = numGrid(1, gridTypeY);
    k = cellIdx/(nx*ny);
    cellIdx -= k*nx*ny;
    k += startIndex(2, gridTypeZ);
    j = cellIdx/nx+startIndex(1, gridTypeY);
    i = cellIdx%nx+startIndex(0, gridTypeX);
} // unwrapIndex

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, const uvec &spanIdx) const {
    switch (this->domain().numDim()) {
        case 1:
            return wrapIndex(loc, spanIdx[0]);
        case 2:
            return wrapIndex(loc, spanIdx[0], spanIdx[1]);
        case 3:
            return wrapIndex(loc, spanIdx[0], spanIdx[1], spanIdx[2]);
        default:
            REPORT_ERROR("Wrong span index array size!");
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, int i) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 1);
#endif
    i -= startIndex(0, gridTypes(0, 0, loc));
    if (this->domain().axisStartBndType(0) == PERIODIC) {
        if (i >= endIndex(0, gridTypes(0, 0, loc))) {
            i -= numGrid(0, gridTypes(0, 0, loc));
        }
    }
    return i;
} // wrapIndex

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, int i, int j) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 2);
#endif
    int nx = numGrid(0, gridTypes(0, 0, loc));
    i -= startIndex(0, gridTypes(0, 0, loc));
    j -= startIndex(1, gridTypes(0, 1, loc));
    if (this->domain().axisStartBndType(0) == PERIODIC) {
        if (i >= endIndex(0, gridTypes(0, 0, loc))) {
            i -= numGrid(0, gridTypes(0, 0, loc));
        }
    }
    if (this->domain().axisStartBndType(1) == PERIODIC) {
        if (j >= endIndex(1, gridTypes(0, 1, loc))) {
            j -= numGrid(1, gridTypes(0, 1, loc));
        }
    }
    return i+nx*j;
} // wrapIndex

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, int i, int j, int k) const {
#ifndef NDEBUG
    assert(this->domain().numDim() == 3);
#endif
    int nx = numGrid(0, gridTypes(0, 0, loc));
    int ny = numGrid(1, gridTypes(0, 1, loc));
    i -= startIndex(0, gridTypes(0, 0, loc));
    j -= startIndex(1, gridTypes(0, 1, loc));
    k -= startIndex(2, gridTypes(0, 2, loc));
    if (this->domain().axisStartBndType(0) == PERIODIC) {
        if (i >= endIndex(0, gridTypes(0, 0, loc))) {
            i -= numGrid(0, gridTypes(0, 0, loc));
        }
    }
    if (this->domain().axisStartBndType(1) == PERIODIC) {
        if (j >= endIndex(1, gridTypes(0, 1, loc))) {
            j -= numGrid(1, gridTypes(0, 1, loc));
        }
    }
    if (this->domain().axisStartBndType(2) == PERIODIC) {
        if (k >= endIndex(1, gridTypes(0, 1, loc))) {
            k -= numGrid(2, gridTypes(0, 2, loc));
        }
    }
    return i+nx*j+nx*ny*k;
} // wrapIndex

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
setGridCoords() {
    // Store the coordinates of each grid point for convenience.
    for (int loc = 0; loc < 8; ++loc) {
        switch (this->domain().numDim()) {
            case 1:
                if (loc != Location::CENTER && loc != Location::VERTEX) continue;
                break;
            case 2:
                if (loc != Location::CENTER && loc != Location::VERTEX &&
                    loc != Location::X_FACE && loc != Location::Y_FACE &&
                    loc != Location::XY_VERTEX) continue;
                break;
            default:
                break;
        }
        gridCoords[loc].set_size(totalNumGrid(loc, this->domain().numDim()));
        for (uword cellIdx = 0; cellIdx < gridCoords[loc].size(); ++cellIdx) {
            gridCoords[loc][cellIdx].setNumDim(this->domain().numDim());
            uvec spanIdx = unwrapIndex(loc, cellIdx);
            for (uword m = 0; m < this->domain().numDim(); ++m) {
                gridCoords[loc][cellIdx].setCoordComp(m, gridCoordComp(m, gridTypes(0, m, loc), spanIdx[m]));
            }
        }
    }
} // setGridCoords

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
setGridTypes() {
    gridTypes.set_size(2, 3, 8);
    gridTypes(0, 0, Location::CENTER) = GridType::FULL;
    gridTypes(0, 1, Location::CENTER) = GridType::FULL;
    gridTypes(0, 2, Location::CENTER) = GridType::FULL;
    gridTypes(1, 0, Location::CENTER) = GridType::HALF;
    gridTypes(1, 1, Location::CENTER) = GridType::HALF;
    gridTypes(1, 2, Location::CENTER) = GridType::HALF;
    gridTypes(0, 0, Location::VERTEX) = GridType::HALF;
    gridTypes(0, 1, Location::VERTEX) = GridType::HALF;
    gridTypes(0, 2, Location::VERTEX) = GridType::HALF;
    gridTypes(1, 0, Location::VERTEX) = GridType::FULL;
    gridTypes(1, 1, Location::VERTEX) = GridType::FULL;
    gridTypes(1, 2, Location::VERTEX) = GridType::FULL;
    gridTypes(0, 0, Location::X_FACE) = GridType::HALF;
    gridTypes(0, 1, Location::X_FACE) = GridType::FULL;
    gridTypes(0, 2, Location::X_FACE) = GridType::FULL;
    gridTypes(1, 0, Location::X_FACE) = GridType::FULL;
    gridTypes(1, 1, Location::X_FACE) = GridType::HALF;
    gridTypes(1, 2, Location::X_FACE) = GridType::HALF;
    gridTypes(0, 0, Location::Y_FACE) = GridType::FULL;
    gridTypes(0, 1, Location::Y_FACE) = GridType::HALF;
    gridTypes(0, 2, Location::Y_FACE) = GridType::FULL;
    gridTypes(1, 0, Location::Y_FACE) = GridType::HALF;
    gridTypes(1, 1, Location::Y_FACE) = GridType::FULL;
    gridTypes(1, 2, Location::Y_FACE) = GridType::HALF;
    gridTypes(0, 0, Location::Z_FACE) = GridType::FULL;
    gridTypes(0, 1, Location::Z_FACE) = GridType::FULL;
    gridTypes(0, 2, Location::Z_FACE) = GridType::HALF;
    gridTypes(1, 0, Location::Z_FACE) = GridType::HALF;
    gridTypes(1, 1, Location::Z_FACE) = GridType::HALF;
    gridTypes(1, 2, Location::Z_FACE) = GridType::FULL;
    gridTypes(0, 0, Location::XY_VERTEX) = GridType::HALF;
    gridTypes(0, 1, Location::XY_VERTEX) = GridType::HALF;
    gridTypes(0, 2, Location::XY_VERTEX) = GridType::FULL;
    gridTypes(1, 0, Location::XY_VERTEX) = GridType::FULL;
    gridTypes(1, 1, Location::XY_VERTEX) = GridType::FULL;
    gridTypes(1, 2, Location::XY_VERTEX) = GridType::HALF;
    gridTypes(0, 0, Location::XZ_VERTEX) = GridType::HALF;
    gridTypes(0, 1, Location::XZ_VERTEX) = GridType::FULL;
    gridTypes(0, 2, Location::XZ_VERTEX) = GridType::HALF;
    gridTypes(1, 0, Location::XZ_VERTEX) = GridType::FULL;
    gridTypes(1, 1, Location::XZ_VERTEX) = GridType::HALF;
    gridTypes(1, 2, Location::XZ_VERTEX) = GridType::FULL;
    gridTypes(0, 0, Location::YZ_VERTEX) = GridType::FULL;
    gridTypes(0, 1, Location::YZ_VERTEX) = GridType::HALF;
    gridTypes(0, 2, Location::YZ_VERTEX) = GridType::HALF;
    gridTypes(1, 0, Location::YZ_VERTEX) = GridType::HALF;
    gridTypes(1, 1, Location::YZ_VERTEX) = GridType::FULL;
    gridTypes(1, 2, Location::YZ_VERTEX) = GridType::FULL;
} // setGridTypes

} // geomtk
