#include "StructuredMesh.h"

StructuredMesh::StructuredMesh(Domain &domain) : Mesh(domain) {
    fullCoords = new vec[domain.getNumDim()];
    halfCoords = new vec[domain.getNumDim()];
    fullIntervals = new vec[domain.getNumDim()];
    halfIntervals = new vec[domain.getNumDim()];
    equidistant = new bool[domain.getNumDim()];
    for (int i = 0; i < domain.getNumDim(); ++i) {
        equidistant[i] = true;
    }
}

StructuredMesh::~StructuredMesh() {
    delete [] fullCoords;
    delete [] halfCoords;
    delete [] fullIntervals;
    delete [] halfIntervals;
    delete [] equidistant;
}

void StructuredMesh::setGridCoords(int dim, int size, double *full, double *half) {
    // sanity check
    if (dim >= domain->getNumDim()) {
        REPORT_ERROR("Argument dim (" << dim << ") exceeds domain dimension (" << domain->getNumDim() << ")!");
    }
    if (domain->getAxisStartBndType(dim) == PERIODIC) {
        fullCoords[dim].resize(size+2);
        halfCoords[dim].resize(size+2);
        fullIntervals[dim].resize(size+1);
        halfIntervals[dim].resize(size+1);
        for (int i = 0; i < size-1; ++i) {
            fullIntervals[dim](i+1) = half[i+1]-half[i];
            halfIntervals[dim](i+1) = full[i+1]-full[i];
        }
        for (int i = 0; i < size; ++i) {
            fullCoords[dim](i+1) = full[i];
            halfCoords[dim](i+1) = half[i];
        }
        // add virtual grids at two ends for periodic boundary condition
        if (full[0] == domain->getAxisStart(dim)) {
            fullIntervals[dim](size) = domain->getAxisEnd(dim)-full[size-1];
            fullIntervals[dim](0) = fullIntervals[dim](size);
            halfIntervals[dim](0) = domain->getAxisSpan(dim)-half[size-1]+half[0];
            halfIntervals[dim](size) = halfIntervals[dim](0);
            fullCoords[dim](0) = domain->getAxisStart(dim)-fullIntervals[dim](size);
            fullCoords[dim](size+1) = domain->getAxisEnd(dim);
            halfCoords[dim](0) = half[0]-halfIntervals[dim](0);
            halfCoords[dim](size+1) = half[size-1]+halfIntervals[dim](size);
        } else if (half[0] == domain->getAxisStart(dim)) {
            fullIntervals[dim](0) = domain->getAxisSpan(dim)-full[size-1]+full[0];
            fullIntervals[dim](size) = fullIntervals[dim](0);
            halfIntervals[dim](size) = domain->getAxisEnd(dim)-half[size-1];
            halfIntervals[dim](0) = halfIntervals[dim](size);
            fullCoords[dim](0) = full[0]-fullIntervals[dim](0);
            fullCoords[dim](size+1) = full[size-1]+fullIntervals[dim](size);
            halfCoords[dim](0) = domain->getAxisStart(dim)-halfIntervals[dim](size);
            halfCoords[dim](size+1) = domain->getAxisEnd(dim);
        } else {
            REPORT_ERROR("Don't know how to handle input grid coordinates of dimension " << dim << "!");
        }
    } else if (domain->getAxisStartBndType(dim) == POLE ||
               domain->getAxisStartBndType(dim) == RIGID) {
        if (full[0] == domain->getAxisStart(dim) &&
            full[size-1] == domain->getAxisEnd(dim)) {
            fullCoords[dim].resize(size);
            halfCoords[dim].resize(size-1);
            fullIntervals[dim].resize(size);
            halfIntervals[dim].resize(size-1);
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i) = full[i];
            }
            for (int i = 0; i < size-1; ++i) {
                halfCoords[dim](i) = half[i];
            }
            for (int i = 1; i < size-1; ++i) {
                fullIntervals[dim](i) = half[i]-half[i-1];
            }
            fullIntervals[dim](0) = half[0]-domain->getAxisStart(dim);
            fullIntervals[dim](size-1) = domain->getAxisEnd(dim)-half[size-2];
            for (int i = 0; i < size-1; ++i) {
                halfIntervals[dim](i) = full[i+1]-full[i];
            }
        } else if (half[0] == domain->getAxisStart(dim) &&
                   half[size] == domain->getAxisEnd(dim)) {
            fullCoords[dim].resize(size);
            halfCoords[dim].resize(size+1);
            fullIntervals[dim].resize(size);
            halfIntervals[dim].resize(size+1);
            for (int i = 0; i < size; ++i) {
                fullCoords[dim](i) = full[i];
            }
            for (int i = 0; i < size+1; ++i) {
                halfCoords[dim](i) = half[i];
            }
            for (int i = 0; i < size; ++i) {
                fullIntervals[dim](i) = half[i+1]-half[i];
            }
            for (int i = 1; i < size; ++i) {
                halfIntervals[dim](i) = half[i+1]-half[i];
            }
            halfIntervals[dim](0) = full[0]-domain->getAxisStart(dim);
            halfIntervals[dim](size) = domain->getAxisEnd(dim)-full[size-1];
        } else {
            REPORT_ERROR("Unhandled branch!");
        }
    } else if (domain->getAxisStartBndType(dim) == INVALID) {
        REPORT_ERROR("Axis " << dim << " is not set!");
    } else {
        REPORT_ERROR("Unhandled branch!");
    }
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
    // check if grids are equidistant
    for (int i = 1; i < fullIntervals[dim].size(); ++i) {
        if (fullIntervals[dim](i) != fullIntervals[dim](0)) {
            equidistant[dim] = false;
            break;
        }
    }
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
