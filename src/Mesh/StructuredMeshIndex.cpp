#include "StructuredMeshIndex.h"

namespace geomtk {

StructuredMeshIndex::StructuredMeshIndex(int numDim) : MeshIndex(numDim) {
    this->numDim = numDim;
    // NOTE: Index is 3D no matter the dimension size of domain.
    indices = new int*[3];
    for (int i = 0; i < 3; ++i) {
        indices[i] = new int[2];
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
        indices[m][GridType::FULL] = UNDEFINED_MESH_INDEX;
        indices[m][GridType::HALF] = UNDEFINED_MESH_INDEX;
    }
    // set the extra dimension indices to 0
    for (int m = numDim; m < 3; ++m) {
        indices[m][GridType::FULL] = 0;
        indices[m][GridType::HALF] = 0;
    }
}

int StructuredMeshIndex::operator()(int axisIdx, int gridType) const {
    return indices[axisIdx][gridType];
}

int& StructuredMeshIndex::operator()(int axisIdx, int gridType) {
    return indices[axisIdx][gridType];
}

StructuredMeshIndex& StructuredMeshIndex::operator=(const StructuredMeshIndex &other) {
    MeshIndex::operator=(other);
    if (this != &other) {
        numDim = other.numDim;
        for (int m = 0; m < 3; ++m) {
            for (int i = 0; i < 2; ++i) {
                indices[m][i] = other.indices[m][i];
            }
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
        if (indices[m][GridType::FULL] == UNDEFINED_MESH_INDEX) {
            if (domain.getAxisStartBndType(m) == PERIODIC) {
                for (int i = 0; i < mesh.getNumGrid(m, GridType::FULL); ++i) {
                    // cout << setw(5) << i;
                    // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i);
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i+1) << endl;
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
                        break;
                    }
                }
                for (int i = indices[m][GridType::FULL]-1;
                     i < indices[m][GridType::FULL]+1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < mesh.getNumGrid(m, GridType::FULL)-1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
                        break;
                    }
                }
                if (domain.getAxisStartBndType(m) == POLE) {
                    if (x(m) < mesh.getGridCoordComp(m, GridType::HALF, 0)) {
                        indices[m][GridType::HALF] = -1;
                        continue;
                    } else if (x(m) > mesh.getGridCoordComp(m, GridType::HALF,
                                                            mesh.getNumGrid(m, GridType::HALF)-1)) {
                        indices[m][GridType::HALF] =
                            mesh.getNumGrid(m, GridType::HALF)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][GridType::FULL]-2, 0);
                int i2 = min(indices[m][GridType::FULL]+2,
                             mesh.getNumGrid(m, GridType::FULL)-1);
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
        } else {
            if (domain.getAxisStartBndType(m) == PERIODIC) {
                if (x(m) < mesh.getGridCoordComp(m, GridType::FULL,
                                                 indices[m][GridType::FULL])) {
                    for (int i = indices[m][GridType::FULL]-1; i >= 0; --i) {
                        // cout << setw(5) << i;
                        // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i);
                        // cout << setw(10) << x(m);
                        // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i+1) << endl;
                        if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.getGridCoordComp(m, GridType::FULL,
                                                        indices[m][GridType::FULL]+1)) {
                    for (int i = indices[m][GridType::FULL]+1;
                         i < mesh.getNumGrid(m, GridType::FULL); ++i) {
                        if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                }
                for (int i = indices[m][GridType::FULL]-1;
                     i < indices[m][GridType::FULL]+1; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            } else {
                if (x(m) < mesh.getGridCoordComp(m, GridType::FULL,
                                                 indices[m][GridType::FULL])) {
                    for (int i = indices[m][GridType::FULL]-1; i >= 0; --i) {
                        // cout << setw(5) << i;
                        // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i);
                        // cout << setw(10) << x(m);
                        // cout << setw(10) << mesh.getGridCoord(m, GridType::FULL, i+1) << endl;
                        if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.getGridCoordComp(m, GridType::FULL,
                                                        indices[m][GridType::FULL]+1)) {
                    for (int i = indices[m][GridType::FULL]+1;
                         i < mesh.getNumGrid(m, GridType::FULL); ++i) {
                        if (x(m) >= mesh.getGridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.getGridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                }
                if (domain.getAxisStartBndType(m) == POLE) {
                    if (x(m) < mesh.getGridCoordComp(m, GridType::HALF, 0)) {
                        indices[m][GridType::HALF] = -1;
                        continue;
                    } else if (x(m) > mesh.getGridCoordComp(m, GridType::HALF,
                                                            mesh.getNumGrid(m, GridType::HALF)-1)) {
                        indices[m][GridType::HALF] =
                            mesh.getNumGrid(m, GridType::HALF)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][GridType::FULL]-2, 0);
                int i2 = min(indices[m][GridType::FULL]+2,
                             mesh.getNumGrid(m, GridType::FULL)-1);
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.getGridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.getGridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
        }
#ifndef NDEBUG
        assert(indices[m][GridType::FULL] != UNDEFINED_MESH_INDEX);
        assert(indices[m][GridType::HALF] != UNDEFINED_MESH_INDEX);
#endif
    }
}

void StructuredMeshIndex::print() const {
    cout << "Center indices:";
    for (int m = 0; m < 3; ++m) {
        cout << setw(10) << indices[m][GridType::FULL];
    }
    cout << endl;
    cout << "Edge indices:  ";
    for (int m = 0; m < 3; ++m) {
        cout << setw(10) << indices[m][GridType::HALF];
    }
    cout << endl;
}

}
