#include "StructuredMesh.h"
#include "StructuredMeshIndex.h"

namespace geomtk {

StructuredMesh::StructuredMesh(Domain &domain) : Mesh(domain) {
    fullCoords = new vec[domain.getNumDim()];
    halfCoords = new vec[domain.getNumDim()];
    fullIntervals = new vec[domain.getNumDim()];
    halfIntervals = new vec[domain.getNumDim()];
    equidistant = new bool[domain.getNumDim()];
    for (int m = 0; m < domain.getNumDim(); ++m) {
        fullCoords[m].resize(0);
        equidistant[m] = true;
    }
}

StructuredMesh::~StructuredMesh() {
    delete [] fullCoords;
    delete [] halfCoords;
    delete [] fullIntervals;
    delete [] halfIntervals;
    delete [] equidistant;
}

void StructuredMesh::setGridCoords(int dim, int size,
                                   double *full, double *half) {
    // sanity check
    if (dim >= domain->getNumDim()) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds domain dimension ("
                     << domain->getNumDim() << ")!");
    }
    // -------------------------------------------------------------------------
    if (domain->getAxisStartBndType(dim) == PERIODIC) {
        fullCoords[dim].resize(size+2);
        halfCoords[dim].resize(size+2);
        fullIntervals[dim].resize(size+1);
        halfIntervals[dim].resize(size+1);
        if (full[0] == domain->getAxisStart(dim)) {
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
            fullCoords[dim](0) = full[0]-domain->getAxisEnd(dim)+full[size-1];
            halfCoords[dim](0) = half[size-1]-domain->getAxisSpan(dim);
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i+1) = full[i];
                halfCoords[dim](i+1) = half[i];
            }
            fullCoords[dim](size+1) = domain->getAxisEnd(dim);
            halfCoords[dim](size+1) = domain->getAxisSpan(dim)+half[0];
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullIntervals[dim](i) = fullCoords[dim](i+1)-fullCoords[dim](i);
            }
            fullIntervals[dim](size) = fullIntervals[dim](0);
            for (int i = 1; i < size+1; ++i) {
                halfIntervals[dim](i) = halfCoords[dim](i)-halfCoords[dim](i-1);
            }
            halfIntervals[dim](0) = halfIntervals[dim](size);
        } else if (half[0] == domain->getAxisStart(dim)) {
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
            fullCoords[dim](0) = full[size-1]-domain->getAxisSpan(dim);
            halfCoords[dim](0) = half[0]-domain->getAxisEnd(dim)+half[size-1];
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i+1) = full[i];
                halfCoords[dim](i+1) = half[i];
            }
            fullCoords[dim](size+1) = domain->getAxisSpan(dim)+full[0];
            halfCoords[dim](size+1) = domain->getAxisEnd(dim);
            // =================================================================
            for (int i = 1; i < size+1; ++i) {
                fullIntervals[dim](i) = fullCoords[dim](i)-fullCoords[dim](i-1);
            }
            fullIntervals[dim](0) = fullIntervals[dim](size);
            for (int i = 0; i < size; ++i) {
                halfIntervals[dim](i) = halfCoords[dim](i+1)-halfCoords[dim](i);
            }
            halfIntervals[dim](size) = halfIntervals[dim](0);
        } else {
            REPORT_ERROR("Don't know how to handle input grid coordinates "
                         "of dimension " << dim << "!");
        }
    }
    // -------------------------------------------------------------------------
    else if (domain->getAxisStartBndType(dim) == POLE ||
             domain->getAxisStartBndType(dim) == RIGID) {
        if (full[0] == domain->getAxisStart(dim) &&
            full[size-1] == domain->getAxisEnd(dim)) {
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
            fullCoords[dim].resize(size);
            halfCoords[dim].resize(size-1);
            fullIntervals[dim].resize(size-1);
            halfIntervals[dim].resize(size);
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i) = full[i];
            }
            for (int i = 0; i < size-1; ++i) {
                halfCoords[dim](i) = half[i];
            }
            // =================================================================
            for (int i = 0; i < size-1; ++i) {
                fullIntervals[dim](i) = full[i+1]-full[i];
            }
            for (int i = 1; i < size-1; ++i) {
                halfIntervals[dim](i) = half[i]-half[i-1];
            }
            halfIntervals[dim](0) = half[0]-domain->getAxisStart(dim);
            halfIntervals[dim](size-1) = domain->getAxisEnd(dim)-half[size-2];
        } else if (half[0] == domain->getAxisStart(dim) &&
                   half[size] == domain->getAxisEnd(dim)) {
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
            fullCoords[dim].resize(size);
            halfCoords[dim].resize(size+1);
            fullIntervals[dim].resize(size+1);
            halfIntervals[dim].resize(size);
            // =================================================================
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i) = full[i];
            }
            for (int i = 0; i < size+1; ++i) {
                halfCoords[dim](i) = half[i];
            }
            // =================================================================
            for (int i = 1; i < size; ++i) {
                fullIntervals[dim](i) = full[i]-full[i-1];
            }
            fullIntervals[dim](0) = full[0]-domain->getAxisStart(dim);
            fullIntervals[dim](size) = domain->getAxisEnd(dim)-full[size-1];
            for (int i = 0; i < size; ++i) {
                halfIntervals[dim](i) = half[i+1]-half[i];
            }
        } else {
            REPORT_ERROR("Unhandled branch!");
        }
    }
    // -------------------------------------------------------------------------
    else if (domain->getAxisStartBndType(dim) == INVALID) {
        REPORT_ERROR("Axis " << dim << " is not set!");
    }
    // -------------------------------------------------------------------------
    else {
        REPORT_ERROR("Unhandled branch!");
    }
    // -------------------------------------------------------------------------
    // check if grids are equidistant
    for (int i = 1; i < fullIntervals[dim].size(); ++i) {
        if (fullIntervals[dim](i) != fullIntervals[dim](0)) {
            equidistant[dim] = false;
            break;
        }
    }
    for (int i = 1; i < halfIntervals[dim].size(); ++i) {
        if (halfIntervals[dim](i) != halfIntervals[dim](0)) {
            equidistant[dim] = false;
            break;
        }
    }
    // -------------------------------------------------------------------------
    // mark the set status
    set = true;
    for (int m = 0; m < domain->getNumDim(); ++m) {
        if (fullCoords[m].size() == 0) {
            set = false;
            break;
        }
    }
}

void StructuredMesh::setGridCoords(int dim, const string &fileName,
                                   const string &fullName,
                                   const string &halfName) {
    // read coordinates from netCDF file
    int ncId, fullDimId, halfDimId, fullVarId, halfVarId;
    size_t fullSize, halfSize;
    double *full, *half;

    if (nc_open(fileName.c_str(), NC_NOCLOBBER, &ncId) != NC_NOERR) {
        REPORT_ERROR("Failed to open " << fileName << "!");
    }
    if (nc_inq_dimid(ncId, fullName.c_str(), &fullDimId) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire dimension " << fullName <<
                     " in " << fileName << "!");
    }
    if (nc_inq_dimlen(ncId, fullDimId, &fullSize) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire dimension length of " << fullName <<
                     " in " << fileName << "!");
    }
    full = new double[fullSize];
    if (nc_inq_dimid(ncId, halfName.c_str(), &halfDimId) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire dimension " << halfName <<
                     " in " << fileName << "!");
    }
    if (nc_inq_dimlen(ncId, halfDimId, &halfSize) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire dimension length of " << halfName <<
                     " in " << fileName << "!");
    }
    half = new double[halfSize];
    if (nc_inq_varid(ncId, fullName.c_str(), &fullVarId) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire variable " << fullName <<
                     " in " << fileName << "!");
    }
    if (nc_get_var(ncId, fullVarId, full) != NC_NOERR) {
        REPORT_ERROR("Failed to get variable " << fullName <<
                     " in " << fileName << "!");
    }
    if (nc_inq_varid(ncId, halfName.c_str(), &halfVarId) != NC_NOERR) {
        REPORT_ERROR("Failed to inquire variable " << halfName <<
                     " in " << fileName << "!");
    }
    if (nc_get_var(ncId, halfVarId, half) != NC_NOERR) {
        REPORT_ERROR("Failed to get variable " << halfName <<
                     " in " << fileName << "!");
    }

    setGridCoords(dim, static_cast<int>(fullSize), full, half);
}

vec StructuredMesh::getGridCoords(int dim, StaggerType staggerType,
                                  bool hasVirtualGrids) const {
    // sanity check
    if (dim >= domain->getNumDim()) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds domain dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(dim) == PERIODIC && !hasVirtualGrids) {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim](span(1, fullCoords[dim].size()-2));
            case EDGE: case VERTEX:
                return halfCoords[dim](span(1, halfCoords[dim].size()-2));
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    } else {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim];
            case EDGE: case VERTEX:
                return halfCoords[dim];
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    }
}

double StructuredMesh::getGridCoord(int dim, StaggerType staggerType, int i) const {
    // sanity check
    if (dim >= domain->getNumDim()) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds domain dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(dim) == PERIODIC) {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim](i+1);
            case EDGE: case VERTEX:
                return halfCoords[dim](i+1);
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    } else {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim](i);
            case EDGE: case VERTEX:
                return halfCoords[dim](i);
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    }
}

void StructuredMesh::getGridCoord(StaggerType staggerType,
                                  const StructuredMeshIndex &idx,
                                  SpaceCoord &x) const {
    for (int m = 0; m < domain->getNumDim(); ++m) {
        x(m) = getGridCoord(m, staggerType, idx(m, staggerType));
    }
}

double StructuredMesh::getGridInterval(int dim, StaggerType staggerType,
                                       int i) const {
    // sanity check
    if (dim >= domain->getNumDim()) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds domain dimension ("
                     << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(dim) == PERIODIC) {
        switch (staggerType) {
            case CENTER:
                return fullIntervals[dim](i+1);
            case EDGE: case VERTEX:
                return halfIntervals[dim](i+1);
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    } else {
        switch (staggerType) {
            case CENTER:
                return fullIntervals[dim](i);
            case EDGE: case VERTEX:
                return halfIntervals[dim](i);
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    }
}

int StructuredMesh::getNumGrid(int dim, StaggerType staggerType,
                               bool hasVirtualGrids) const {
    // sanity check
    if (dim >= domain->getNumDim()) {
        return 1;
    }
    if (domain->getAxisStartBndType(dim) == PERIODIC && !hasVirtualGrids) {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim].size()-2;
            case EDGE: case VERTEX:
                return halfCoords[dim].size()-2;
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    } else {
        switch (staggerType) {
            case CENTER:
                return fullCoords[dim].size();
            case EDGE: case VERTEX:
                return halfCoords[dim].size();
            default:
                REPORT_ERROR("Unknown stagger type!");
        }
    }
}

bool StructuredMesh::isEquidistant(int dim) const {
    return equidistant[dim];
}

}
