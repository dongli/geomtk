namespace geomtk {

template <class MeshType, class CoordType>
StructuredMeshIndex<MeshType, CoordType>::
StructuredMeshIndex() : MeshIndex<MeshType, CoordType>() {
    indices = NULL;
}

template <class MeshType, class CoordType>
StructuredMeshIndex<MeshType, CoordType>::
StructuredMeshIndex(uword numDim) : MeshIndex<MeshType, CoordType>(numDim) {
    init(numDim);
}

template <class MeshType, class CoordType>
StructuredMeshIndex<MeshType, CoordType>::
~StructuredMeshIndex() {
    if (indices) {
        for (uword i = 0; i < 3; ++i) {
            delete [] indices[i];
        }
        delete [] indices;
    }
}

template <class MeshType, class CoordType>
void StructuredMeshIndex<MeshType, CoordType>::
init(uword numDim) {
    MeshIndex<MeshType, CoordType>::init(numDim);
    // NOTE: Index is 3D no matter the dimension size of domain.
    indices = new int*[3];
    for (uword i = 0; i < 3; ++i) {
        indices[i] = new int[2];
    }
    reset();
} // init

template <class MeshType, class CoordType>
void StructuredMeshIndex<MeshType, CoordType>::
reset() {
    for (uword m = 0; m < this->numDim; ++m) {
        indices[m][GridType::FULL] = UNDEFINED_MESH_INDEX;
        indices[m][GridType::HALF] = UNDEFINED_MESH_INDEX;
    }
    // Set the extra dimension indices to 0.
    for (int m = this->numDim; m < 3; ++m) {
        indices[m][GridType::FULL] = 0;
        indices[m][GridType::HALF] = 0;
    }
} // reset

template <class MeshType, class CoordType>
int StructuredMeshIndex<MeshType, CoordType>::
operator()(uword axisIdx, int gridType) const {
    return indices[axisIdx][gridType];
} // operator()

template <class MeshType, class CoordType>
int& StructuredMeshIndex<MeshType, CoordType>::
operator()(uword axisIdx, int gridType) {
    return indices[axisIdx][gridType];
} // operator()

template <class MeshType, class CoordType>
StructuredMeshIndex<MeshType, CoordType>& StructuredMeshIndex<MeshType, CoordType>::
operator=(const StructuredMeshIndex<MeshType, CoordType> &other) {
    MeshIndex<MeshType, CoordType>::operator=(other);
    if (this != &other) {
        for (uword m = 0; m < 3; ++m) {
            for (uword i = 0; i < 2; ++i) {
                indices[m][i] = other.indices[m][i];
            }
        }
    }
    return *this;
} // operator=

// TODO: Could we use the searching results by using MLPACK?
template <class MeshType, class CoordType>
void StructuredMeshIndex<MeshType, CoordType>::
locate(const MeshType &mesh, const CoordType &x) {
    const auto &domain = mesh.domain();
#ifndef NDEBUG
    assert(domain.isValid(x));
#endif
    for (uword m = 0; m < domain.numDim(); ++m) {
        // #####################################################################
        if (indices[m][GridType::FULL] == UNDEFINED_MESH_INDEX) {
            if (domain.axisStartBndType(m) == PERIODIC) {
                for (uword i = mesh.startIndex(m, GridType::FULL);
                     i <= mesh.endIndex(m, GridType::FULL); ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
                        break;
                    }
                }
                for (int i = indices[m][GridType::FULL]-1;
                     i < indices[m][GridType::FULL]+1; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
            // =================================================================
            else if (mesh.gridCoordComp(m, GridType::FULL, 0) == domain.axisStart(m)) {
                for (uword i = mesh.startIndex(m, GridType::FULL);
                     i < mesh.endIndex(m, GridType::FULL); ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
                        break;
                    }
                }
                // TODO: Could we remove the POLE condition?
                if (domain.axisStartBndType(m) == POLE) {
                    if (x(m) < mesh.gridCoordComp(m, GridType::HALF,
                                                  mesh.startIndex(m, GridType::HALF))) {
                        indices[m][GridType::HALF] = -1;
                        continue;
                    } else if (x(m) > mesh.gridCoordComp(m, GridType::HALF,
                                                         mesh.endIndex(m, GridType::HALF))) {
                        indices[m][GridType::HALF] = mesh.numGrid(m, GridType::HALF)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][GridType::FULL]-2, static_cast<int>(mesh.startIndex(m, GridType::FULL)));
                int i2 = min(indices[m][GridType::FULL]+2, static_cast<int>(mesh.endIndex(m, GridType::FULL)));
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
            // =================================================================
            else {
                for (int i = static_cast<int>(mesh.startIndex(m, GridType::HALF));
                     i < static_cast<int>(mesh.endIndex(m, GridType::HALF)); ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
                if (x(m) < mesh.gridCoordComp(m, GridType::FULL,
                                              mesh.startIndex(m, GridType::FULL))) {
                    indices[m][GridType::FULL] = -1;
                    continue;
                } else if (x(m) > mesh.gridCoordComp(m, GridType::FULL,
                                                     mesh.endIndex(m, GridType::FULL))) {
                    indices[m][GridType::FULL] = mesh.numGrid(m, GridType::FULL)-1;
                    continue;
                }
                int i1 = max(indices[m][GridType::HALF]-2, static_cast<int>(mesh.startIndex(m, GridType::HALF)));
                int i2 = min(indices[m][GridType::HALF]+2, static_cast<int>(mesh.endIndex(m, GridType::HALF)));
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
                        break;
                    }
                }
            }
        }
        // #####################################################################
        else {
            if (domain.axisStartBndType(m) == PERIODIC) {
                if (x(m) < mesh.gridCoordComp(m, GridType::FULL,
                                              indices[m][GridType::FULL])) {
                    for (int i = indices[m][GridType::FULL]-1;
                         i >= static_cast<int>(mesh.startIndex(m, GridType::FULL)); --i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.gridCoordComp(m, GridType::FULL,
                                                     indices[m][GridType::FULL]+1)) {
                    for (int i = indices[m][GridType::FULL]+1;
                         i <= static_cast<int>(mesh.endIndex(m, GridType::FULL)); ++i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                }
                for (int i = indices[m][GridType::FULL]-1;
                     i < indices[m][GridType::FULL]+1; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
            // =================================================================
            else if (mesh.gridCoordComp(m, GridType::FULL, 0) == domain.axisStart(m)) {
                if (x(m) < mesh.gridCoordComp(m, GridType::FULL,
                                              indices[m][GridType::FULL])) {
                    for (int i = indices[m][GridType::FULL]-1;
                         i >= static_cast<int>(mesh.startIndex(m, GridType::FULL)); --i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.gridCoordComp(m, GridType::FULL,
                                                     indices[m][GridType::FULL]+1)) {
                    for (int i = indices[m][GridType::FULL]+1;
                         i <= static_cast<int>(mesh.endIndex(m, GridType::FULL)); ++i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                            indices[m][GridType::FULL] = i;
                            break;
                        }
                    }
                }
                // TODO: Could we remove the POLE condition?
                if (domain.axisStartBndType(m) == POLE) {
                    if (x(m) < mesh.gridCoordComp(m, GridType::HALF,
                                                  mesh.startIndex(m, GridType::HALF))) {
                        indices[m][GridType::HALF] = -1;
                        continue;
                    } else if (x(m) > mesh.gridCoordComp(m, GridType::HALF,
                                                         mesh.endIndex(m, GridType::HALF))) {
                        indices[m][GridType::HALF] = mesh.numGrid(m, GridType::HALF)-1;
                        continue;
                    }
                }
                int i1 = max(indices[m][GridType::FULL]-2, static_cast<int>(mesh.startIndex(m, GridType::FULL)));
                int i2 = min(indices[m][GridType::FULL]+2, static_cast<int>(mesh.endIndex(m, GridType::FULL)));
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                        indices[m][GridType::HALF] = i;
                        break;
                    }
                }
            }
            // =================================================================
            else {
                if (x(m) < mesh.gridCoordComp(m, GridType::HALF,
                                              indices[m][GridType::HALF])) {
                    for (int i = indices[m][GridType::HALF]-1;
                         i >= static_cast<int>(mesh.startIndex(m, GridType::HALF)); --i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                            indices[m][GridType::HALF] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.gridCoordComp(m, GridType::HALF,
                                                     indices[m][GridType::HALF]+1)) {
                    for (int i = indices[m][GridType::HALF]+1;
                         i <= static_cast<int>(mesh.endIndex(m, GridType::HALF)); ++i) {
                        if (x(m) >= mesh.gridCoordComp(m, GridType::HALF, i) &&
                            x(m) <= mesh.gridCoordComp(m, GridType::HALF, i+1)) {
                            indices[m][GridType::HALF] = i;
                            break;
                        }
                    }
                }
                    if (x(m) < mesh.gridCoordComp(m, GridType::FULL,
                                                  mesh.startIndex(m, GridType::FULL))) {
                        indices[m][GridType::FULL] = -1;
                        continue;
                    } else if (x(m) > mesh.gridCoordComp(m, GridType::FULL,
                                                         mesh.endIndex(m, GridType::FULL))) {
                        indices[m][GridType::FULL] = mesh.numGrid(m, GridType::FULL)-1;
                        continue;
                    }
                int i1 = max(indices[m][GridType::HALF]-2, static_cast<int>(mesh.startIndex(m, GridType::HALF)));
                int i2 = min(indices[m][GridType::HALF]+2, static_cast<int>(mesh.endIndex(m, GridType::HALF)));
                for (int i = i1; i < i2; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, GridType::FULL, i) &&
                        x(m) <= mesh.gridCoordComp(m, GridType::FULL, i+1)) {
                        indices[m][GridType::FULL] = i;
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
} // locate

template <class MeshType, class CoordType>
uword StructuredMeshIndex<MeshType, CoordType>::
cellIndex(const MeshType &mesh, int loc) const {
    int i, j, k;
    switch (mesh.domain().numDim()) {
        case 1:
            switch (loc) {
                case Location::CENTER:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    return mesh.wrapIndex(Location::VERTEX, i);
                case Location::VERTEX:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    return mesh.wrapIndex(Location::CENTER, i);
                default:
                    REPORT_ERROR("Unsupported stagger location!");
            }
        case 2:
            switch (loc) {
                case Location::CENTER:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    return mesh.wrapIndex(Location::VERTEX, i, j);
                case Location::VERTEX:
                case Location::XY_VERTEX:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    return mesh.wrapIndex(Location::CENTER, i, j);
                case Location::X_FACE:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    return mesh.wrapIndex(Location::Y_FACE, i, j);
                case Location::Y_FACE:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    return mesh.wrapIndex(Location::X_FACE, i, j);
                default:
                    REPORT_ERROR("Unsupported stagger location!");
            }
        case 3:
            switch (loc) {
                case Location::CENTER:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    k = mesh.gridStyle(2) == FULL_LEAD ? indices[2][GridType::HALF]+1 : indices[2][GridType::HALF];
                    return mesh.wrapIndex(Location::VERTEX, i, j, k);
                case Location::VERTEX:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    k = mesh.gridStyle(2) == HALF_LEAD ? indices[2][GridType::FULL]+1 : indices[2][GridType::FULL];
                    return mesh.wrapIndex(Location::CENTER, i, j, k);
                case Location::X_FACE:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    k = mesh.gridStyle(2) == FULL_LEAD ? indices[2][GridType::HALF]+1 : indices[2][GridType::HALF];
                    return mesh.wrapIndex(Location::YZ_VERTEX, i, j, k);
                case Location::Y_FACE:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    k = mesh.gridStyle(2) == FULL_LEAD ? indices[2][GridType::HALF]+1 : indices[2][GridType::HALF];
                    return mesh.wrapIndex(Location::XZ_VERTEX, i, j, k);
                case Location::Z_FACE:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    k = mesh.gridStyle(2) == HALF_LEAD ? indices[2][GridType::FULL]+1 : indices[2][GridType::FULL];
                    return mesh.wrapIndex(Location::XY_VERTEX, i, j, k);
                case Location::XY_VERTEX:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    k = mesh.gridStyle(2) == FULL_LEAD ? indices[2][GridType::HALF]+1 : indices[2][GridType::HALF];
                    return mesh.wrapIndex(Location::Z_FACE, i, j, k);
                case Location::XZ_VERTEX:
                    i = mesh.gridStyle(0) == HALF_LEAD ? indices[0][GridType::FULL]+1 : indices[0][GridType::FULL];
                    j = mesh.gridStyle(1) == FULL_LEAD ? indices[1][GridType::HALF]+1 : indices[1][GridType::HALF];
                    k = mesh.gridStyle(2) == HALF_LEAD ? indices[2][GridType::FULL]+1 : indices[2][GridType::FULL];
                    return mesh.wrapIndex(Location::Y_FACE, i, j, k);
                case Location::YZ_VERTEX:
                    i = mesh.gridStyle(0) == FULL_LEAD ? indices[0][GridType::HALF]+1 : indices[0][GridType::HALF];
                    j = mesh.gridStyle(1) == HALF_LEAD ? indices[1][GridType::FULL]+1 : indices[1][GridType::FULL];
                    k = mesh.gridStyle(2) == HALF_LEAD ? indices[2][GridType::FULL]+1 : indices[2][GridType::FULL];
                    return mesh.wrapIndex(Location::X_FACE, i, j, k);
                default:
                    REPORT_ERROR("Unsupported stagger location!");
            }
        default:
            REPORT_ERROR("Incorrect domain dimension!");
    }
} // cellIndex

template <class MeshType, class CoordType>
bool StructuredMeshIndex<MeshType, CoordType>::
isValid() const {
    if (indices[0][GridType::FULL] == UNDEFINED_MESH_INDEX) {
        return false;
    } else {
        return true;
    }
} // isValid

template <class MeshType, class CoordType>
void StructuredMeshIndex<MeshType, CoordType>::
print() const {
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
} // print

} // geomtk
