namespace geomtk {

template <class DomainType, class CoordType>
StructuredMesh<DomainType, CoordType>::StructuredMesh(DomainType &domain)
        : Mesh<DomainType, CoordType>(domain) {
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
StructuredMesh<DomainType, CoordType>::~StructuredMesh() {
    delete [] fullCoords;
    delete [] halfCoords;
    delete [] fullIntervals;
    delete [] halfIntervals;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::init(const string &fileName) {
    setGridCoords();
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::init(const string &fileNameH, const string &fileNameV) {
    setGridCoords();
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::init(int nx, int ny, int nz) {
    vec full, half;
    int n[3] = {nx, ny, nz};
    double dx;
    for (int m = 0; m < this->domain().numDim(); ++m) {
        if (this->domain().axisStartBndType(m) == PERIODIC) {
            full.resize(n[m]);
            half.resize(n[m]);
            dx = this->domain().axisSpan(m)/n[m];
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+i*dx;
                half[i] = full[i]+0.5*dx;
            }
        } else if (this->domain().axisStartBndType(m) == POLE &&
                   this->domain().axisEndBndType(m) == POLE) {
            full.resize(n[m]);
            half.resize(n[m]-1);
            dx = this->domain().axisSpan(m)/(n[m]-1);
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+i*dx;
            }
            for (int i = 0; i < n[m]-1; ++i) {
                half[i] = full[i]+0.5*dx;
            }
        } else if ((this->domain().axisStartBndType(m) == RIGID &&
                    this->domain().axisEndBndType(m) == RIGID) ||
                   (this->domain().axisStartBndType(m) == OPEN &&
                    this->domain().axisEndBndType(m) == OPEN)) {
            full.resize(n[m]);
            half.resize(n[m]+1);
            dx = this->domain().axisSpan(m)/n[m];
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain().axisStart(m)+0.5*dx+i*dx;
            }
            for (int i = 0; i < n[m]+1; ++i) {
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
setGridCoordComps(int axisIdx, int size, const vec &full, const vec &half) {
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
        fullCoords[axisIdx].set_size(size+2);
        halfCoords[axisIdx].set_size(size+2);
        fullIntervals[axisIdx].set_size(size+1);
        halfIntervals[axisIdx].set_size(size+1);
        if (fabs(full[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            /*
             o - full grid   0 - virtual full grid
             * - half grid   x - virtual half grid
             
             size = 4
              _______________________________________
             |                                       |
             |    0         1         2         3    |    4         5
             0         1         2         3         4         5
             0----x----o----*----o----*----o----*----o----*----0----x
                       |                                       |
                       |_______________________________________|
                     start                                    end
             -----------
            fullIntervals
                  -----------
                 halfIntervals
             */
            fullCoords[axisIdx](0) = full[0]-this->domain().axisEnd(axisIdx)+full[size-1];
            halfCoords[axisIdx](0) = half[size-1]-this->domain().axisSpan(axisIdx);
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            // Note: Make sure this first full grid coordinate is equal with the
            // axis starting coordinate to ease the following judging.
            fullCoords[axisIdx](1) = this->domain().axisStart(axisIdx);
            fullCoords[axisIdx](size+1) = this->domain().axisEnd(axisIdx);
            halfCoords[axisIdx](size+1) = this->domain().axisSpan(axisIdx)+half[0];
            for (int i = 0; i < size; ++i) {
                fullIntervals[axisIdx](i) = fullCoords[axisIdx](i+1)-fullCoords[axisIdx](i);
            }
            fullIntervals[axisIdx](size) = fullIntervals[axisIdx](0);
            for (int i = 1; i < size+1; ++i) {
                halfIntervals[axisIdx](i) = halfCoords[axisIdx](i)-halfCoords[axisIdx](i-1);
            }
            halfIntervals[axisIdx](0) = halfIntervals[axisIdx](size);
        } else if (fabs(half[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            /*
             o - full grid   0 - virtual full grid
             * - half grid   x - virtual half grid
             
             size = 4
              _______________________________________
             |                                       |
             |    0         1         2         3    |    4         5
             0         1         2         3         4         5
             x----0----*----o----*----o----*----o----*----o----x----0
                       |                                       |
                       |_______________________________________|
                     start                                    end
             -----------
            fullIntervals
                  -----------
                 halfIntervals
             */
            fullCoords[axisIdx](0) = full[size-1]-this->domain().axisSpan(axisIdx);
            halfCoords[axisIdx](0) = half[0]-this->domain().axisEnd(axisIdx)+half[size-1];
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            // Note: Make sure this first half grid coordinate is equal with the
            // axis starting coordinate to ease the following judging.
            halfCoords[axisIdx](1) = this->domain().axisStart(axisIdx);
            fullCoords[axisIdx](size+1) = this->domain().axisSpan(axisIdx)+full[0];
            halfCoords[axisIdx](size+1) = this->domain().axisEnd(axisIdx);
            for (int i = 1; i < size+1; ++i) {
                fullIntervals[axisIdx](i) = fullCoords[axisIdx](i)-fullCoords[axisIdx](i-1);
            }
            fullIntervals[axisIdx](0) = fullIntervals[axisIdx](size);
            for (int i = 0; i < size; ++i) {
                halfIntervals[axisIdx](i) = halfCoords[axisIdx](i+1)-halfCoords[axisIdx](i);
            }
            halfIntervals[axisIdx](size) = halfIntervals[axisIdx](0);
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
                o - full grid
                * - half grid
             
                size = 6
             
                     0         1         2         3         4
                0         1         2         3         4         5
                o----*----o----*----o----*----o----*----o----*----o
                |                                                 |
              start                                              end
                -----------
               fullIntervals
                     -----------
                    halfIntervals
             
             */
            fullCoords[axisIdx].set_size(size);
            halfCoords[axisIdx].set_size(size-1);
            fullIntervals[axisIdx].set_size(size-1);
            halfIntervals[axisIdx].set_size(size);
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (int i = 0; i < size-1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // Note: Make sure the both ends of the full grid coordinates are
            // equal with the axis starting and ending coordiante to ease the
            // following judging.
            fullCoords[axisIdx](0) = this->domain().axisStart(axisIdx);
            fullCoords[axisIdx](size-1) = this->domain().axisEnd(axisIdx);
            for (int i = 0; i < size-1; ++i) {
                fullIntervals[axisIdx](i) = full[i+1]-full[i];
            }
            for (int i = 1; i < size-1; ++i) {
                halfIntervals[axisIdx](i) = half[i]-half[i-1];
            }
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
               halfIntervals
                     -----------
                    fullIntervals
             */
            fullCoords[axisIdx].set_size(size);
            halfCoords[axisIdx].set_size(size+1);
            fullIntervals[axisIdx].set_size(size+1);
            halfIntervals[axisIdx].set_size(size);
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (int i = 0; i < size+1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // Note: Make sure the both ends of the half grid coordinates are
            // equal with the axis starting and ending coordiante to ease the
            // following judging.
            halfCoords[axisIdx](0) = this->domain().axisStart(axisIdx);
            halfCoords[axisIdx](size) = this->domain().axisEnd(axisIdx);
            for (int i = 1; i < size; ++i) {
                fullIntervals[axisIdx](i) = full[i]-full[i-1];
            }
            fullIntervals[axisIdx](0) = full[0]-this->domain().axisStart(axisIdx);
            fullIntervals[axisIdx](size) = this->domain().axisEnd(axisIdx)-full[size-1];
            for (int i = 0; i < size; ++i) {
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
    for (int m = 0; m < this->domain().numDim(); ++m) {
        if (fullCoords[m].size() == 0) {
            return;
        }
    }
    for (int m = 0; m < this->domain().numDim(); ++m) {
        // Set the default grid index ranges.
        if (this->domain().axisStartBndType(m) == PERIODIC) {
            fullIndexRanges(0, m) = 1;
            fullIndexRanges(1, m) = fullCoords[m].size()-2;
            halfIndexRanges(0, m) = 1;
            halfIndexRanges(1, m) = halfCoords[m].size()-2;
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
    setCellVolumes();
    setGridCoords();
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::is(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::ie(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::js(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::je(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::ks(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::ke(int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::
startIndex(int axisIdx, int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::
endIndex(int axisIdx, int gridType) const {
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
int StructuredMesh<DomainType, CoordType>::levelIndex(int i, int loc) const {
    int idx[3];
    unwrapIndex(loc, i, idx);
    return idx[2];
}

template <class DomainType, class CoordType>
bool StructuredMesh<DomainType, CoordType>::isHorizontalGridsSame(const Mesh<DomainType, CoordType> &_other) const {
    if (this == &_other) {
        return true;
    }
    if (this->type() != _other.type()) {
        return false;
    }
    const StructuredMesh<DomainType, CoordType> &other = static_cast<const StructuredMesh<DomainType, CoordType>&>(_other);
    const double eps = 1.0e-14;
    for (int m = 0; m < 2; ++m) {
        if (this->fullCoords[m].size() != other.fullCoords[m].size()) {
            return false;
        }
        for (int i = 0; i < this->fullCoords[m].size(); ++i) {
            if (fabs(this->fullCoords[m][i]-other.fullCoords[m][i]) > eps) {
                return false;
            }
        }
        if (this->halfCoords[m].size() != other.halfCoords[m].size()) {
            return false;
        }
        for (int i = 0; i < this->halfCoords[m].size(); ++i) {
            if (fabs(this->halfCoords[m][i]-other.halfCoords[m][i]) > eps) {
                return false;
            }
        }
    }
    return true;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::setGridCoordComps(int axisIdx, int size, const vec &full) {
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
            for (int i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
            half[size-1] = (full[size-1]+this->domain().axisEnd(axisIdx))*0.5;
        } else if (full[size-1] == this->domain().axisEnd(axisIdx)) {
            half[0] = (this->domain().axisStart(axisIdx)+full[0])*0.5;
            for (int i = 1; i < size; ++i) {
                half[i] = (full[i-1]+full[i])*0.5;
            }
        }
    } else if (this->domain().axisStartBndType(axisIdx) == POLE ||
               this->domain().axisStartBndType(axisIdx) == RIGID ||
               this->domain().axisStartBndType(axisIdx) == OPEN) {

        if (fabs(full[0]-this->domain().axisStart(axisIdx)) < 1.0e-14) {
            assert(fabs(full[size-1]-this->domain().axisEnd(axisIdx)) < 1.0e-14);
            half.set_size(size-1);
            for (int i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
        } else {
            half.set_size(size+1);
            half[0] = this->domain().axisStart(axisIdx);
            for (int i = 1; i < size; ++i) {
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
gridCoordComps(int axisIdx, int gridType, bool hasVirtualGrids) const {
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx](span(1, fullCoords[axisIdx].size()-2));
            case GridType::HALF:
                return halfCoords[axisIdx](span(1, halfCoords[axisIdx].size()-2));
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
gridCoordComp(int axisIdx, int gridType, int gridIdx) const {
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
    return gridCoords[loc][i];
}

template <class DomainType, class CoordType>
double StructuredMesh<DomainType, CoordType>::
gridInterval(int axisIdx, int gridType, int gridIdx) const {
    if (axisIdx >= this->domain().numDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain().numDim() << ")!");
    }
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC) {
        switch (gridType) {
            case GridType::FULL:
                return fullIntervals[axisIdx](gridIdx+1);
            case GridType::HALF:
                return halfIntervals[axisIdx](gridIdx+1);
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    } else {
        switch (gridType) {
            case GridType::FULL:
                return fullIntervals[axisIdx](gridIdx);
            case GridType::HALF:
                return halfIntervals[axisIdx](gridIdx);
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
totalNumGrid(int loc, int numDim) const {
    if (numDim == -1) {
        numDim = this->domain().numDim();
    }
#ifndef NDEBUG
    assert(numDim > 1 && numDim <= 3);
#endif
    switch (loc) {
        case Location::CENTER:
            if (numDim == 2) {
                return numGrid(0, GridType::FULL)*
                       numGrid(1, GridType::FULL);
            } else {
                return numGrid(0, GridType::FULL)*
                       numGrid(1, GridType::FULL)*
                       numGrid(2, GridType::FULL);
            }
        case Location::X_FACE:
            if (numDim == 2) {
                return numGrid(0, GridType::HALF)*
                       numGrid(1, GridType::FULL);
            } else {
                return numGrid(0, GridType::HALF)*
                       numGrid(1, GridType::FULL)*
                       numGrid(2, GridType::FULL);
            }
        case Location::Y_FACE:
            if (numDim == 2) {
                return numGrid(0, GridType::FULL)*
                       numGrid(1, GridType::HALF);
            } else {
                return numGrid(0, GridType::FULL)*
                       numGrid(1, GridType::HALF)*
                       numGrid(2, GridType::FULL);
            }
        case Location::Z_FACE:
            return numGrid(0, GridType::FULL)*
                   numGrid(1, GridType::FULL)*
                   numGrid(2, GridType::HALF);
        case Location::XY_VERTEX:
            if (numDim == 2) {
                return numGrid(0, GridType::HALF)*
                       numGrid(1, GridType::HALF);
            } else {
                return numGrid(0, GridType::HALF)*
                       numGrid(1, GridType::HALF)*
                       numGrid(2, GridType::FULL);
            }
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
numGrid(int axisIdx, int gridType, bool hasVirtualGrids) const {
    if (axisIdx >= this->domain().numDim()) {
        return 1;
    }
    if (this->domain().axisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx].size()-2;
            case GridType::HALF:
                return halfCoords[axisIdx].size()-2;
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
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::
unwrapIndex(int loc, int cellIdx, int gridIdx[3]) const {
    int nx, ny;
    switch (loc) {
        case Location::CENTER:
        case Location::Z_FACE:
            nx = numGrid(0, GridType::FULL);
            ny = numGrid(1, GridType::FULL);
            gridIdx[2] = cellIdx/(nx*ny);
            cellIdx -= gridIdx[2]*nx*ny;
            gridIdx[1] = cellIdx/nx;
            gridIdx[0] = cellIdx%nx;
            gridIdx[0] += fullIndexRanges(0, 0);
            gridIdx[1] += fullIndexRanges(0, 1);
            gridIdx[2] += fullIndexRanges(0, 2);
            break;
        case Location::X_FACE:
            nx = numGrid(0, GridType::HALF);
            ny = numGrid(1, GridType::FULL);
            gridIdx[2] = cellIdx/(nx*ny);
            cellIdx -= gridIdx[2]*nx*ny;
            gridIdx[1] = cellIdx/nx;
            gridIdx[0] = cellIdx%nx;
            gridIdx[0] += halfIndexRanges(0, 0);
            gridIdx[1] += fullIndexRanges(0, 1);
            gridIdx[2] += fullIndexRanges(0, 2);
            break;
        case Location::Y_FACE:
            nx = numGrid(0, GridType::FULL);
            ny = numGrid(1, GridType::HALF);
            gridIdx[2] = cellIdx/(nx*ny);
            cellIdx -= gridIdx[2]*nx*ny;
            gridIdx[1] = cellIdx/nx;
            gridIdx[0] = cellIdx%nx;
            gridIdx[0] += fullIndexRanges(0, 0);
            gridIdx[1] += halfIndexRanges(0, 1);
            gridIdx[2] += fullIndexRanges(0, 2);
            break;
        case Location::XY_VERTEX:
            nx = numGrid(0, GridType::HALF);
            ny = numGrid(1, GridType::HALF);
            gridIdx[2] = cellIdx/(nx*ny);
            cellIdx -= gridIdx[2]*nx*ny;
            gridIdx[1] = cellIdx/nx;
            gridIdx[0] = cellIdx%nx;
            gridIdx[0] += halfIndexRanges(0, 0);
            gridIdx[1] += halfIndexRanges(0, 1);
            gridIdx[2] += fullIndexRanges(0, 2);
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, int i, int j, int k) const {
    int nx, ny, res;
    switch (loc) {
        case Location::CENTER:
            nx = numGrid(0, GridType::FULL);
            ny = numGrid(1, GridType::FULL);
            i -= fullIndexRanges(0, 0);
            j -= fullIndexRanges(0, 1);
            k -= fullIndexRanges(0, 2);
            break;
        case Location::X_FACE:
            nx = numGrid(0, GridType::HALF);
            ny = numGrid(1, GridType::FULL);
            i -= halfIndexRanges(0, 0);
            j -= fullIndexRanges(0, 1);
            k -= fullIndexRanges(0, 2);
            break;
        case Location::Y_FACE:
            nx = numGrid(0, GridType::FULL);
            ny = numGrid(1, GridType::HALF);
            i -= fullIndexRanges(0, 0);
            j -= halfIndexRanges(0, 1);
            k -= fullIndexRanges(0, 2);
            break;
        case Location::XY_VERTEX:
            nx = numGrid(0, GridType::HALF);
            ny = numGrid(1, GridType::HALF);
            i -= halfIndexRanges(0, 0);
            j -= halfIndexRanges(0, 1);
            k -= fullIndexRanges(0, 2);
            break;
        case Location::Z_FACE:
            REPORT_ERROR("Under construction!");
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
    res = (nx*ny)*k;
    res += nx*j;
    res += i;
    return res;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::setGridCoords() {
    // Store the coordinates of each grid point for convenience.
    int gridIdx[3];
    for (int loc = 0; loc < 5; ++loc) {
        gridCoords[loc].set_size(totalNumGrid(loc, this->domain().numDim()));
        for (int cellIdx = 0; cellIdx < gridCoords[loc].size(); ++cellIdx) {
            gridCoords[loc][cellIdx].setNumDim(this->domain().numDim());
            unwrapIndex(loc, cellIdx, gridIdx);
            for (int m = 0; m < this->domain().numDim(); ++m) {
                if ((m == 0 && (loc == Location::X_FACE || loc == Location::XY_VERTEX)) ||
                    (m == 1 && (loc == Location::Y_FACE || loc == Location::XY_VERTEX)) ||
                    (m == 2 && loc == Location::Z_FACE)) {
                    gridCoords[loc][cellIdx].setCoordComp(m, gridCoordComp(m, GridType::HALF, gridIdx[m]));
                } else {
                    gridCoords[loc][cellIdx].setCoordComp(m, gridCoordComp(m, GridType::FULL, gridIdx[m]));
                }
            }
        }
    }
}

} // geomtk
