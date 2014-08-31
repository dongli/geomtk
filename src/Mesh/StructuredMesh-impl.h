namespace geomtk {

template <class DomainType, class CoordType>
StructuredMesh<DomainType, CoordType>::StructuredMesh(DomainType &domain)
        : Mesh<DomainType, CoordType>(domain) {
    fullCoords = new vec[domain.getNumDim()];
    halfCoords = new vec[domain.getNumDim()];
    fullIntervals = new vec[domain.getNumDim()];
    halfIntervals = new vec[domain.getNumDim()];
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
    for (int m = 0; m < this->domain->getNumDim(); ++m) {
        if (this->domain->getAxisStartBndType(m) == PERIODIC) {
            full.resize(n[m]);
            half.resize(n[m]);
            dx = this->domain->getAxisSpan(m)/n[m];
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain->getAxisStart(m)+i*dx;
                half[i] = full[i]+0.5*dx;
            }
        } else if (this->domain->getAxisStartBndType(m) == POLE &&
                   this->domain->getAxisEndBndType(m) == POLE) {
            full.resize(n[m]);
            half.resize(n[m]-1);
            dx = this->domain->getAxisSpan(m)/(n[m]-1);
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain->getAxisStart(m)+i*dx;
            }
            for (int i = 0; i < n[m]-1; ++i) {
                half[i] = full[i]+0.5*dx;
            }
        } else if (this->domain->getAxisStartBndType(m) == RIGID &&
                   this->domain->getAxisEndBndType(m) == RIGID) {
            full.resize(n[m]);
            half.resize(n[m]+1);
            dx = this->domain->getAxisSpan(m)/n[m];
            for (int i = 0; i < n[m]; ++i) {
                full[i] = this->domain->getAxisStart(m)+0.5*dx+i*dx;
            }
            for (int i = 0; i < n[m]+1; ++i) {
                half[i] = this->domain->getAxisStart(m)+i*dx;
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        setGridCoordComps(m, n[m], full, half);
    }
    setCellVolumes();
    setGridCoords();
    this->set = true;
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::setGridCoordComps(int axisIdx, int size, const vec &full, const vec &half) {
    // sanity check
    if (axisIdx >= this->domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain->getNumDim() << ")!");
    }
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC) {
        if (full.size() != half.size()) {
            REPORT_ERROR("Full grid size (" << full.size() << ") should " <<
                         "be equal with half grid (" << half.size() << ")!");
        }
        fullCoords[axisIdx].set_size(size+2);
        halfCoords[axisIdx].set_size(size+2);
        fullIntervals[axisIdx].set_size(size+1);
        halfIntervals[axisIdx].set_size(size+1);
        if (full[0] == this->domain->getAxisStart(axisIdx)) {
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
            fullCoords[axisIdx](0) = full[0]-this->domain->getAxisEnd(axisIdx)+full[size-1];
            halfCoords[axisIdx](0) = half[size-1]-this->domain->getAxisSpan(axisIdx);
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            fullCoords[axisIdx](size+1) = this->domain->getAxisEnd(axisIdx);
            halfCoords[axisIdx](size+1) = this->domain->getAxisSpan(axisIdx)+half[0];
            for (int i = 0; i < size; ++i) {
                fullIntervals[axisIdx](i) = fullCoords[axisIdx](i+1)-fullCoords[axisIdx](i);
            }
            fullIntervals[axisIdx](size) = fullIntervals[axisIdx](0);
            for (int i = 1; i < size+1; ++i) {
                halfIntervals[axisIdx](i) = halfCoords[axisIdx](i)-halfCoords[axisIdx](i-1);
            }
            halfIntervals[axisIdx](0) = halfIntervals[axisIdx](size);
        } else if (half[0] == this->domain->getAxisStart(axisIdx)) {
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
            fullCoords[axisIdx](0) = full[size-1]-this->domain->getAxisSpan(axisIdx);
            halfCoords[axisIdx](0) = half[0]-this->domain->getAxisEnd(axisIdx)+half[size-1];
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            fullCoords[axisIdx](size+1) = this->domain->getAxisSpan(axisIdx)+full[0];
            halfCoords[axisIdx](size+1) = this->domain->getAxisEnd(axisIdx);
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
    } else if (this->domain->getAxisStartBndType(axisIdx) == POLE ||
             this->domain->getAxisStartBndType(axisIdx) == RIGID ||
             this->domain->getAxisStartBndType(axisIdx) == OPEN) {
        if (fabs(full[0]-this->domain->getAxisStart(axisIdx)) < 1.0e-14 &&
            fabs(full[size-1]-this->domain->getAxisEnd(axisIdx)) < 1.0e-14) {
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
            for (int i = 0; i < size-1; ++i) {
                fullIntervals[axisIdx](i) = full[i+1]-full[i];
            }
            for (int i = 1; i < size-1; ++i) {
                halfIntervals[axisIdx](i) = half[i]-half[i-1];
            }
            halfIntervals[axisIdx](0) = half[0]-this->domain->getAxisStart(axisIdx);
            halfIntervals[axisIdx](size-1) = this->domain->getAxisEnd(axisIdx)-half[size-2];
        } else if (fabs(half[0]-this->domain->getAxisStart(axisIdx)) < 1.0e-14 &&
                   fabs(half[size]-this->domain->getAxisEnd(axisIdx)) < 1.0e-14) {
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
            for (int i = 1; i < size; ++i) {
                fullIntervals[axisIdx](i) = full[i]-full[i-1];
            }
            fullIntervals[axisIdx](0) = full[0]-this->domain->getAxisStart(axisIdx);
            fullIntervals[axisIdx](size) = this->domain->getAxisEnd(axisIdx)-full[size-1];
            for (int i = 0; i < size; ++i) {
                halfIntervals[axisIdx](i) = half[i+1]-half[i];
            }
        } else {
            REPORT_ERROR("Unhandled branch!");
        }
    } else if (this->domain->getAxisStartBndType(axisIdx) == INVALID) {
        REPORT_ERROR("Axis " << axisIdx << " is not set!");
    } else {
        REPORT_ERROR("Unhandled branch!");
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::getLevelIndex(int i, int loc) const {
    int idx[3];
    unwrapIndex(loc, i, idx);
    return idx[2];
}

template <class DomainType, class CoordType>
bool StructuredMesh<DomainType, CoordType>::isHorizontalGridsSame(const Mesh<DomainType, CoordType> &_other) const {
    if (this == &_other) {
        return true;
    }
    if (this->getType() != _other.getType()) {
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
    if (axisIdx >= this->domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain->getNumDim() << ")!");
    }
    // construct the half grid coordinates from full grids
    vec half;
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC) {
        half.set_size(size);
        if (full[0] == this->domain->getAxisStart(axisIdx)) {
            for (int i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
            half[size-1] = (full[size-1]+this->domain->getAxisEnd(axisIdx))*0.5;
        } else if (full[size-1] == this->domain->getAxisEnd(axisIdx)) {
            half[0] = (this->domain->getAxisStart(axisIdx)+full[0])*0.5;
            for (int i = 1; i < size; ++i) {
                half[i] = (full[i-1]+full[i])*0.5;
            }
        }
    } else if (this->domain->getAxisStartBndType(axisIdx) == POLE ||
               this->domain->getAxisStartBndType(axisIdx) == RIGID ||
               this->domain->getAxisStartBndType(axisIdx) == OPEN) {
        half.set_size(size-1);
        assert(fabs(full[0]-this->domain->getAxisStart(axisIdx)) < 1.0e-14);
        assert(fabs(full[size-1]-this->domain->getAxisEnd(axisIdx)) < 1.0e-14);
        for (int i = 0; i < size-1; ++i) {
            half[i] = (full[i]+full[i+1])*0.5;
        }
    }
    setGridCoordComps(axisIdx, size, full, half);
}

template <class DomainType, class CoordType>
vec StructuredMesh<DomainType, CoordType>::getGridCoordComps(int axisIdx, int gridType, bool hasVirtualGrids) const {
    if (axisIdx >= this->domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain->getNumDim() << ")!");
    }
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
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
double StructuredMesh<DomainType, CoordType>::getGridCoordComp(int axisIdx, int gridType, int gridIdx) const {
    if (axisIdx >= this->domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain->getNumDim() << ")!");
    }
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC) {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx](gridIdx+1);
            case GridType::HALF:
                return halfCoords[axisIdx](gridIdx+1);
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    } else {
        switch (gridType) {
            case GridType::FULL:
                return fullCoords[axisIdx](gridIdx);
            case GridType::HALF:
                return halfCoords[axisIdx](gridIdx);
            default:
                REPORT_ERROR("Unknown grid type!");
        }
    }
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::getGridCoord(int i, int loc, CoordType &x) const {
    // TODO: Set a CoordType array to store the grid coordinates, so we can
    // avoid the duplicate works.
    int idx[3];
    unwrapIndex(loc, i, idx);
    switch (loc) {
        case Location::CENTER:
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, idx[m]));
            }
            break;
        case Location::X_FACE:
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                if (m == 0) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, idx[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, idx[m]));
                }
            }
            break;
        case Location::Y_FACE:
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                if (m == 1) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, idx[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, idx[m]));
                }
            }
            break;
        case Location::Z_FACE:
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                if (m == 2) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, idx[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, idx[m]));
                }
            }
            break;
        case Location::XY_VERTEX:
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                if (m == 0 || m == 1) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, idx[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, idx[m]));
                }
            }
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

template <class DomainType, class CoordType>
const CoordType& StructuredMesh<DomainType, CoordType>::getGridCoord(int i, int loc) const {
    return gridCoords[loc][i];
}

template <class DomainType, class CoordType>
void StructuredMesh<DomainType, CoordType>::setCellVolumes() {
    REPORT_ERROR("Under construction!");
}

template <class DomainType, class CoordType>
double StructuredMesh<DomainType, CoordType>::getCellVolume(int cellIdx) const {
    return volumes(cellIdx);
}

template <class DomainType, class CoordType>
double StructuredMesh<DomainType, CoordType>::getGridInterval(int axisIdx, int gridType,
                                                              int gridIdx) const {
    if (axisIdx >= this->domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << this->domain->getNumDim() << ")!");
    }
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC) {
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
int StructuredMesh<DomainType, CoordType>::getTotalNumGrid(int loc, int numDim) const {
#ifndef NDEBUG
    assert(numDim > 1 && numDim <= 3);
#endif
    switch (loc) {
        case Location::CENTER:
            if (numDim == 2) {
                return getNumGrid(0, GridType::FULL)*
                       getNumGrid(1, GridType::FULL);
            } else {
                return getNumGrid(0, GridType::FULL)*
                       getNumGrid(1, GridType::FULL)*
                       getNumGrid(2, GridType::FULL);
            }
        case Location::X_FACE:
            if (numDim == 2) {
                return getNumGrid(0, GridType::HALF)*
                       getNumGrid(1, GridType::FULL);
            } else {
                return getNumGrid(0, GridType::HALF)*
                       getNumGrid(1, GridType::FULL)*
                       getNumGrid(2, GridType::FULL);
            }
        case Location::Y_FACE:
            if (numDim == 2) {
                return getNumGrid(0, GridType::FULL)*
                       getNumGrid(1, GridType::HALF);
            } else {
                return getNumGrid(0, GridType::FULL)*
                       getNumGrid(1, GridType::HALF)*
                       getNumGrid(2, GridType::FULL);
            }
        case Location::Z_FACE:
            return getNumGrid(0, GridType::FULL)*
                   getNumGrid(1, GridType::FULL)*
                   getNumGrid(2, GridType::HALF);
        case Location::XY_VERTEX:
            if (numDim == 2) {
                return getNumGrid(0, GridType::HALF)*
                       getNumGrid(1, GridType::HALF);
            } else {
                return getNumGrid(0, GridType::HALF)*
                       getNumGrid(1, GridType::HALF)*
                       getNumGrid(2, GridType::FULL);
            }
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::getNumGrid(int axisIdx, int gridType,
                                                      bool hasVirtualGrids) const {
    if (axisIdx >= this->domain->getNumDim()) {
        return 1;
    }
    if (this->domain->getAxisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
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
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::FULL);
            break;
        case Location::X_FACE:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::FULL);
            break;
        case Location::Y_FACE:
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::HALF);
            break;
        case Location::XY_VERTEX:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::HALF);
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
    gridIdx[2] = cellIdx/(nx*ny);
    cellIdx -= gridIdx[2]*nx*ny;
    gridIdx[1] = cellIdx/nx;
    gridIdx[0] = cellIdx%nx;
}

template <class DomainType, class CoordType>
int StructuredMesh<DomainType, CoordType>::
wrapIndex(int loc, int i, int j, int k) const {
    int nx, ny, res;
    switch (loc) {
        case Location::CENTER:
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::FULL);
            break;
        case Location::X_FACE:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::FULL);
            break;
        case Location::Y_FACE:
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::HALF);
            break;
        case Location::XY_VERTEX:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::HALF);
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
        gridCoords[loc].
        set_size(getTotalNumGrid(loc, this->domain->getNumDim()));
        for (int cellIdx = 0; cellIdx < gridCoords[loc].size(); ++cellIdx) {
            gridCoords[loc][cellIdx].setNumDim(this->domain->getNumDim());
            unwrapIndex(loc, cellIdx, gridIdx);
            for (int m = 0; m < this->domain->getNumDim(); ++m) {
                if ((m == 0 && (loc == Location::X_FACE || loc == Location::XY_VERTEX)) ||
                    (m == 1 && (loc == Location::Y_FACE || loc == Location::XY_VERTEX)) ||
                    (m == 2 && loc == Location::Z_FACE)) {
                    gridCoords[loc][cellIdx].
                    setCoordComp(m, getGridCoordComp(m, GridType::HALF, gridIdx[m]));
                } else {
                    gridCoords[loc][cellIdx].
                    setCoordComp(m, getGridCoordComp(m, GridType::FULL, gridIdx[m]));
                }
            }
        }
    }
}

} // geomtk
