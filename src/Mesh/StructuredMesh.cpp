#include "StructuredMesh.h"
#include "StructuredMeshIndex.h"

namespace geomtk {

StructuredMesh::StructuredMesh(Domain &domain) : Mesh(domain) {
    fullCoords = new vec[domain.getNumDim()];
    halfCoords = new vec[domain.getNumDim()];
    fullIntervals = new vec[domain.getNumDim()];
    halfIntervals = new vec[domain.getNumDim()];
}

StructuredMesh::~StructuredMesh() {
    delete [] fullCoords;
    delete [] halfCoords;
    delete [] fullIntervals;
    delete [] halfIntervals;
}

void StructuredMesh::init(const string &fileName) {
    REPORT_ERROR("Under construction!");
    set = true;
}

void StructuredMesh::init(int nx, int ny, int nz) {
    vec full, half;
    int n[3] = {nx, ny, nz};
    double dx;
    for (int m = 0; m < domain->getNumDim(); ++m) {
        if (domain->getAxisStartBndType(m) == PERIODIC) {
            full.resize(n[m]);
            half.resize(n[m]);
            dx = domain->getAxisSpan(m)/n[m];
            for (int i = 0; i < n[m]; ++i) {
                full[i] = domain->getAxisStart(m)+i*dx;
                half[i] = full[i]+0.5*dx;
            }
        } else if (domain->getAxisStartBndType(m) == POLE &&
                   domain->getAxisEndBndType(m) == POLE) {
            full.resize(n[m]);
            half.resize(n[m]-1);
            dx = domain->getAxisSpan(m)/(n[m]-1);
            for (int i = 0; i < n[m]; ++i) {
                full[i] = domain->getAxisStart(m)+i*dx;
            }
            for (int i = 0; i < n[m]-1; ++i) {
                half[i] = full[i]+0.5*dx;
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        setGridCoords(m, n[m], full, half);
    }
    setCellVolumes();
    set = true;
}

void StructuredMesh::setGridCoords(int axisIdx, int size, const vec &full,
                                   const vec &half) {
    // sanity check
    if (axisIdx >= domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << domain->getNumDim() << ")!");
    }
    // -------------------------------------------------------------------------
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC) {
        if (full.size() != half.size()) {
            REPORT_ERROR("Full grid size (" << full.size() << ") should " <<
                         "be equal with half grid (" << half.size() << ")!");
        }
        fullCoords[axisIdx].set_size(size+2);
        halfCoords[axisIdx].set_size(size+2);
        fullIntervals[axisIdx].set_size(size+1);
        halfIntervals[axisIdx].set_size(size+1);
        if (full[0] == domain->getAxisStart(axisIdx)) {
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
            // =================================================================
            fullCoords[axisIdx](0) = full[0]-domain->getAxisEnd(axisIdx)+full[size-1];
            halfCoords[axisIdx](0) = half[size-1]-domain->getAxisSpan(axisIdx);
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            fullCoords[axisIdx](size+1) = domain->getAxisEnd(axisIdx);
            halfCoords[axisIdx](size+1) = domain->getAxisSpan(axisIdx)+half[0];
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullIntervals[axisIdx](i) = fullCoords[axisIdx](i+1)-fullCoords[axisIdx](i);
            }
            fullIntervals[axisIdx](size) = fullIntervals[axisIdx](0);
            for (int i = 1; i < size+1; ++i) {
                halfIntervals[axisIdx](i) = halfCoords[axisIdx](i)-halfCoords[axisIdx](i-1);
            }
            halfIntervals[axisIdx](0) = halfIntervals[axisIdx](size);
        } else if (half[0] == domain->getAxisStart(axisIdx)) {
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
            // =================================================================
            fullCoords[axisIdx](0) = full[size-1]-domain->getAxisSpan(axisIdx);
            halfCoords[axisIdx](0) = half[0]-domain->getAxisEnd(axisIdx)+half[size-1];
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i+1) = full[i];
                halfCoords[axisIdx](i+1) = half[i];
            }
            fullCoords[axisIdx](size+1) = domain->getAxisSpan(axisIdx)+full[0];
            halfCoords[axisIdx](size+1) = domain->getAxisEnd(axisIdx);
            // =================================================================
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
    }
    // -------------------------------------------------------------------------
    else if (domain->getAxisStartBndType(axisIdx) == POLE ||
             domain->getAxisStartBndType(axisIdx) == RIGID ||
             domain->getAxisStartBndType(axisIdx) == OPEN) {
        if (fabs(full[0]-domain->getAxisStart(axisIdx)) < 1.0e-14 &&
            fabs(full[size-1]-domain->getAxisEnd(axisIdx)) < 1.0e-14) {
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
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (int i = 0; i < size-1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // =================================================================
            for (int i = 0; i < size-1; ++i) {
                fullIntervals[axisIdx](i) = full[i+1]-full[i];
            }
            for (int i = 1; i < size-1; ++i) {
                halfIntervals[axisIdx](i) = half[i]-half[i-1];
            }
            halfIntervals[axisIdx](0) = half[0]-domain->getAxisStart(axisIdx);
            halfIntervals[axisIdx](size-1) = domain->getAxisEnd(axisIdx)-half[size-2];
        } else if (fabs(half[0]-domain->getAxisStart(axisIdx)) < 1.0e-14 &&
                   fabs(half[size]-domain->getAxisEnd(axisIdx)) < 1.0e-14) {
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
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullCoords[axisIdx](i) = full[i];
            }
            for (int i = 0; i < size+1; ++i) {
                halfCoords[axisIdx](i) = half[i];
            }
            // =================================================================
            for (int i = 1; i < size; ++i) {
                fullIntervals[axisIdx](i) = full[i]-full[i-1];
            }
            fullIntervals[axisIdx](0) = full[0]-domain->getAxisStart(axisIdx);
            fullIntervals[axisIdx](size) = domain->getAxisEnd(axisIdx)-full[size-1];
            for (int i = 0; i < size; ++i) {
                halfIntervals[axisIdx](i) = half[i+1]-half[i];
            }
        } else {
            REPORT_ERROR("Unhandled branch!");
        }
    }
    // -------------------------------------------------------------------------
    else if (domain->getAxisStartBndType(axisIdx) == INVALID) {
        REPORT_ERROR("Axis " << axisIdx << " is not set!");
    }
    // -------------------------------------------------------------------------
    else {
        REPORT_ERROR("Unhandled branch!");
    }
}

void StructuredMesh::setGridCoords(int axisIdx, int size, const vec &full) {
    // sanity check
    if (axisIdx >= domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << domain->getNumDim() << ")!");
    }
    // construct the half grid coordinates from full grids
    vec half;
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC) {
        half.set_size(size);
        if (full[0] == domain->getAxisStart(axisIdx)) {
            for (int i = 0; i < size-1; ++i) {
                half[i] = (full[i]+full[i+1])*0.5;
            }
            half[size-1] = (full[size-1]+domain->getAxisEnd(axisIdx))*0.5;
        } else if (full[size-1] == domain->getAxisEnd(axisIdx)) {
            half[0] = (domain->getAxisStart(axisIdx)+full[0])*0.5;
            for (int i = 1; i < size; ++i) {
                half[i] = (full[i-1]+full[i])*0.5;
            }
        }
    } else if (domain->getAxisStartBndType(axisIdx) == POLE ||
               domain->getAxisStartBndType(axisIdx) == RIGID ||
               domain->getAxisStartBndType(axisIdx) == OPEN) {
        half.set_size(size-1);
        assert(fabs(full[0]-domain->getAxisStart(axisIdx)) < 1.0e-14);
        assert(fabs(full[size-1]-domain->getAxisEnd(axisIdx)) < 1.0e-14);
        for (int i = 0; i < size-1; ++i) {
            half[i] = (full[i]+full[i+1])*0.5;
        }
    }
    setGridCoords(axisIdx, size, full, half);
}

vec StructuredMesh::getGridCoords(int axisIdx, int gridType,
                                  bool hasVirtualGrids) const {
    if (axisIdx >= domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
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

double StructuredMesh::getGridCoordComp(int axisIdx, int gridType,
                                        int gridIdx) const {
    if (axisIdx >= domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC) {
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

void StructuredMesh::getGridCoord(int cellIdx, int loc, SpaceCoord &x) const {
    int i[3];
    unwrapIndex(cellIdx, i, loc);
    switch (loc) {
        case Location::CENTER:
            for (int m = 0; m < domain->getNumDim(); ++m) {
                x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, i[m]));
            }
            break;
        case Location::X_FACE:
            for (int m = 0; m < domain->getNumDim(); ++m) {
                if (m == 0) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, i[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, i[m]));
                }
            }
            break;
        case Location::Y_FACE:
            for (int m = 0; m < domain->getNumDim(); ++m) {
                if (m == 1) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, i[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, i[m]));
                }
            }
            break;
        case Location::Z_FACE:
            for (int m = 0; m < domain->getNumDim(); ++m) {
                if (m == 2) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, i[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, i[m]));
                }
            }
            break;
        case Location::XY_VERTEX:
            for (int m = 0; m < domain->getNumDim(); ++m) {
                if (m == 0 || m == 1) {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::HALF, i[m]));
                } else {
                    x.setCoordComp(m, getGridCoordComp(m, GridType::FULL, i[m]));
                }
            }
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

void StructuredMesh::setCellVolumes() {
    REPORT_ERROR("Under construction!");
}

double StructuredMesh::getCellVolume(int cellIdx) const {
    int i[3];
    unwrapIndex(cellIdx, i, Location::CENTER);
    return volumes(i[0], i[1], i[2]);
}

double StructuredMesh::getGridInterval(int axisIdx, int gridType,
                                       int gridIdx) const {
    if (axisIdx >= domain->getNumDim()) {
        REPORT_ERROR("Argument axisIdx (" << axisIdx << ") exceeds domain " <<
                     "dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC) {
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
    
int StructuredMesh::getTotalNumGrid(int loc) const {
    switch (loc) {
        case Location::CENTER:
            return getNumGrid(0, GridType::FULL)*
                   getNumGrid(1, GridType::FULL)*
                   getNumGrid(2, GridType::FULL);
        case Location::X_FACE:
            return getNumGrid(0, GridType::HALF)*
                   getNumGrid(1, GridType::FULL)*
                   getNumGrid(2, GridType::FULL);
        case Location::Y_FACE:
            return getNumGrid(0, GridType::FULL)*
                   getNumGrid(1, GridType::HALF)*
                   getNumGrid(2, GridType::FULL);
        case Location::Z_FACE:
            return getNumGrid(0, GridType::FULL)*
                   getNumGrid(1, GridType::FULL)*
                   getNumGrid(2, GridType::HALF);
        case Location::XY_VERTEX:
            return getNumGrid(0, GridType::HALF)*
                   getNumGrid(1, GridType::HALF)*
                   getNumGrid(2, GridType::FULL);
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
}

int StructuredMesh::getNumGrid(int axisIdx, int gridType,
                               bool hasVirtualGrids) const {
    if (axisIdx >= domain->getNumDim()) {
        return 1;
    }
    if (domain->getAxisStartBndType(axisIdx) == PERIODIC && !hasVirtualGrids) {
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

void StructuredMesh::unwrapIndex(int cellIdx, int i[3], int loc) const {
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
    i[2] = cellIdx/(nx*ny);
    cellIdx -= i[2]*nx*ny;
    i[1] = cellIdx/nx;
    i[0] = cellIdx%nx;
}

int StructuredMesh::wrapIndex(const StructuredMeshIndex &meshIdx,
                              int loc) const {
    int nx, ny, res;
    switch (loc) {
        case Location::CENTER:
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::FULL);
            res = (nx*ny)*meshIdx(2, GridType::FULL);
            res += nx*meshIdx(1, GridType::FULL);
            res += meshIdx(0, GridType::FULL);
            break;
        case Location::X_FACE:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::FULL);
            res = (nx*ny)*meshIdx(2, GridType::FULL);
            res += nx*meshIdx(1, GridType::HALF);
            res += meshIdx(0, GridType::FULL);
            break;
        case Location::Y_FACE:
            nx = getNumGrid(0, GridType::FULL);
            ny = getNumGrid(1, GridType::HALF);
            res = (nx*ny)*meshIdx(2, GridType::FULL);
            res += nx*meshIdx(1, GridType::FULL);
            res += meshIdx(0, GridType::HALF);
            break;
        case Location::XY_VERTEX:
            nx = getNumGrid(0, GridType::HALF);
            ny = getNumGrid(1, GridType::HALF);
            res = (nx*ny)*meshIdx(2, GridType::FULL);
            res += nx*meshIdx(1, GridType::HALF);
            res += meshIdx(0, GridType::HALF);
            break;
        case Location::Z_FACE:
            REPORT_ERROR("Under construction!");
            break;
        default:
            REPORT_ERROR("Unknown stagger location!");
    }
    return res;
}

}
