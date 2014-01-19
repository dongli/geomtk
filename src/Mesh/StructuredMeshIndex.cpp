#include "StructuredMeshIndex.h"

namespace geomtk {

StructuredMeshIndex::StructuredMeshIndex(int numDim) : MeshIndex(numDim) {
    this->numDim = numDim;
    // NOTE: Index is 3D no matter the dimension size of domain.
    indices = new int*[3];
    for (int i = 0; i < 3; ++i) {
        indices[i] = new int[3];
    }
    reset();
}

StructuredMeshIndex::~StructuredMeshIndex() {
    for (int i = 0; i < 3; ++i) {
        delete [] indices[i];
    }
    delete [] indices;
}

void StructuredMeshIndex::reset() {
    for (int m = 0; m < numDim; ++m) {
        indices[m][CENTER] = UNDEFINED_MESH_INDEX;
        indices[m][EDGE] = UNDEFINED_MESH_INDEX;
        indices[m][VERTEX] = UNDEFINED_MESH_INDEX;
    }
    // set the extra dimension indices to 0
    for (int m = numDim; m < 3; ++m) {
        indices[m][CENTER] = 0;
        indices[m][EDGE] = 0;
        indices[m][VERTEX] = 0;
    }
}

int StructuredMeshIndex::operator()(int dim, StaggerType staggerType) const {
    return indices[dim][staggerType];
}

int& StructuredMeshIndex::operator()(int dim, StaggerType staggerType) {
    return indices[dim][staggerType];
}

StructuredMeshIndex& StructuredMeshIndex::operator=(const StructuredMeshIndex &other) {
    MeshIndex::operator=(other);
    if (this != &other) {
        numDim = other.numDim;
        for (int m = 0; m < 3; ++m) {
            indices[m][CENTER] = other.indices[m][CENTER];
            indices[m][EDGE] = other.indices[m][EDGE];
            indices[m][VERTEX] = other.indices[m][VERTEX];
        }
    }
    return *this;
}

void StructuredMeshIndex::locate(const Mesh &mesh_, const SpaceCoord &x) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(mesh_);
    const Domain &domain = mesh.getDomain();
    for (int m = 0; m < domain.getNumDim(); ++m) {
        // sanity check
        if (x(m) < domain.getAxisStart(m) ||
            x(m) > domain.getAxisEnd(m)) {
            REPORT_ERROR("Coordinate x is out of range on dimension " << m << "!");
        }
        if (indices[m][CENTER] == UNDEFINED_MESH_INDEX) {
            if (domain.getAxisStartBndType(m) == PERIODIC) {
                for (int i = 0; i < mesh.getNumGrid(m, CENTER); ++i) {
                    // cout << setw(5) << i;
                    // cout << setw(10) << mesh.getGridCoord(m, CENTER, i);
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.getGridCoord(m, CENTER, i+1) << endl;
                    if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                        x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                        indices[m][CENTER] = i;
                        break;
                    }
                }
                for (int i = indices[m][CENTER]-1; i < indices[m][CENTER]+1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, EDGE, i) &&
                        x(m) <= mesh.getGridCoordComp(m, EDGE, i+1)) {
                        indices[m][EDGE] = i;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < mesh.getNumGrid(m, CENTER)-1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                        x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                        indices[m][CENTER] = i;
                        break;
                    }
                }
                if (domain.getAxisStartBndType(m) == POLE) {
                    if (x(m) < mesh.getGridCoordComp(m, EDGE, 0)) {
                        indices[m][EDGE] = -1;
                        continue;
                    } else if (x(m) > mesh.getGridCoordComp(m, EDGE, mesh.getNumGrid(m, EDGE)-1)) {
                        indices[m][EDGE] = mesh.getNumGrid(m, EDGE)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][CENTER]-2, 0);
                int i2 = min(indices[m][CENTER]+2, mesh.getNumGrid(m, CENTER)-1);
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, EDGE, i) &&
                        x(m) <= mesh.getGridCoordComp(m, EDGE, i+1)) {
                        indices[m][EDGE] = i;
                        break;
                    }
                }
            }
        } else {
            if (domain.getAxisStartBndType(m) == PERIODIC) {
                if (x(m) < mesh.getGridCoordComp(m, CENTER, indices[m][CENTER])) {
                    for (int i = indices[m][CENTER]-1; i >= 0; --i) {
                        // cout << setw(5) << i;
                        // cout << setw(10) << mesh.getGridCoord(m, CENTER, i);
                        // cout << setw(10) << x(m);
                        // cout << setw(10) << mesh.getGridCoord(m, CENTER, i+1) << endl;
                        if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                            x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                            indices[m][CENTER] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.getGridCoordComp(m, CENTER, indices[m][CENTER]+1)) {
                    for (int i = indices[m][CENTER]+1; i < mesh.getNumGrid(m, CENTER); ++i) {
                        if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                            x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                            indices[m][CENTER] = i;
                            break;
                        }
                    }
                }
                for (int i = indices[m][CENTER]-1; i < indices[m][CENTER]+1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, EDGE, i) &&
                        x(m) <= mesh.getGridCoordComp(m, EDGE, i+1)) {
                        indices[m][EDGE] = i;
                        break;
                    }
                }
            } else {
                if (x(m) < mesh.getGridCoordComp(m, CENTER, indices[m][CENTER])) {
                    for (int i = indices[m][CENTER]-1; i >= 0; --i) {
                        // cout << setw(5) << i;
                        // cout << setw(10) << mesh.getGridCoord(m, CENTER, i);
                        // cout << setw(10) << x(m);
                        // cout << setw(10) << mesh.getGridCoord(m, CENTER, i+1) << endl;
                        if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                            x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                            indices[m][CENTER] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.getGridCoordComp(m, CENTER, indices[m][CENTER]+1)) {
                    for (int i = indices[m][CENTER]+1; i < mesh.getNumGrid(m, CENTER); ++i) {
                        if (x(m) >= mesh.getGridCoordComp(m, CENTER, i) &&
                            x(m) <= mesh.getGridCoordComp(m, CENTER, i+1)) {
                            indices[m][CENTER] = i;
                            break;
                        }
                    }
                }
                if (domain.getAxisStartBndType(m) == POLE) {
                    if (x(m) < mesh.getGridCoordComp(m, EDGE, 0)) {
                        indices[m][EDGE] = -1;
                        continue;
                    } else if (x(m) > mesh.getGridCoordComp(m, EDGE, mesh.getNumGrid(m, EDGE)-1)) {
                        indices[m][EDGE] = mesh.getNumGrid(m, EDGE)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][CENTER]-2, 0);
                int i2 = min(indices[m][CENTER]+2, mesh.getNumGrid(m, CENTER)-1);
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, EDGE, i) &&
                        x(m) <= mesh.getGridCoordComp(m, EDGE, i+1)) {
                        indices[m][EDGE] = i;
                        break;
                    }
                }
            }
        }
        assert(indices[m][CENTER] != UNDEFINED_MESH_INDEX);
        assert(indices[m][EDGE] != UNDEFINED_MESH_INDEX);
    }
}

void StructuredMeshIndex::print() const {
    cout << "Center indices:";
    for (int m = 0; m < 3; ++m) {
        cout << setw(10) << indices[m][CENTER];
    }
    cout << endl;
    cout << "Edge indices:  ";
    for (int m = 0; m < 3; ++m) {
        cout << setw(10) << indices[m][EDGE];
    }
    cout << endl;
}

}
