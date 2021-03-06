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
                int gridType1, gridType2;
                if (mesh.gridStyle(m) == FULL_LEAD) {
                    gridType1 = GridType::FULL;
                    gridType2 = GridType::HALF;
                } else {
                    gridType1 = GridType::HALF;
                    gridType2 = GridType::FULL;
                }
                for (uword i = mesh.startIndex(m, gridType1);
                     i <= mesh.endIndex(m, gridType1); ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, gridType1, i) &&
                        x(m) <= mesh.gridCoordComp(m, gridType1, i+1)) {
                        indices[m][gridType1] = i;
                        break;
                    }
                }
#ifndef NDEBUG
                assert(indices[m][gridType1] != UNDEFINED_MESH_INDEX);
#endif
                for (int i = indices[m][gridType1]-1;
                     i < indices[m][gridType1]+1; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, gridType2, i) &&
                        x(m) <= mesh.gridCoordComp(m, gridType2, i+1)) {
                        indices[m][gridType2] = i;
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
                int gridType1, gridType2;
                if (mesh.gridStyle(m) == FULL_LEAD) {
                    gridType1 = GridType::FULL;
                    gridType2 = GridType::HALF;
                } else {
                    gridType1 = GridType::HALF;
                    gridType2 = GridType::FULL;
                }
                if (x(m) < mesh.gridCoordComp(m, gridType1, indices[m][gridType1])) {
                    for (int i = indices[m][gridType1]-1;
                         i >= static_cast<int>(mesh.startIndex(m, gridType1)); --i) {
                        if (x(m) >= mesh.gridCoordComp(m, gridType1, i) &&
                            x(m) <= mesh.gridCoordComp(m, gridType1, i+1)) {
                            indices[m][gridType1] = i;
                            break;
                        }
                    }
                } else if (x(m) > mesh.gridCoordComp(m, gridType1, indices[m][gridType1]+1)) {
                    for (int i = indices[m][gridType1]+1;
                         i <= static_cast<int>(mesh.endIndex(m, gridType1)); ++i) {
                        if (x(m) >= mesh.gridCoordComp(m, gridType1, i) &&
                            x(m) <= mesh.gridCoordComp(m, gridType1, i+1)) {
                            indices[m][gridType1] = i;
                            break;
                        }
                    }
                }
                for (int i = indices[m][gridType1]-1; i < indices[m][gridType1]+1; ++i) {
                    if (x(m) >= mesh.gridCoordComp(m, gridType2, i) &&
                        x(m) <= mesh.gridCoordComp(m, gridType2, i+1)) {
                        indices[m][gridType2] = i;
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
    uvec::fixed<3> spanIdx;
    for (uword m = 0; m < mesh.domain().numDim(); ++m) {
        auto gridStyle = mesh.gridStyle(m);
        auto gridType = mesh.gridType(m, loc, true);
        if ((gridStyle == FULL_LEAD && gridType == GridType::HALF) ||
            (gridStyle == HALF_LEAD && gridType == GridType::FULL)) {
            spanIdx[m] = indices[m][gridType]+1;
        } else {
            spanIdx[m] = indices[m][gridType];
        }
    }
    return mesh.wrapIndex(mesh.dualGridLocation(loc), spanIdx);
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
bool StructuredMeshIndex<MeshType, CoordType>::
atBoundary(const MeshType &mesh) const {
    bool res = false;
    for (uword m = 0; m < this->numDim; ++m) {
        if (mesh.domain().axisStartBndType(m) != RIGID) continue;
        if (res) break;
        auto gridStyle = mesh.gridStyle(m);
        if (gridStyle == FULL_LEAD) {
            res = indices[m][GridType::FULL] == mesh.startIndex(m, GridType::FULL) ||
                  indices[m][GridType::FULL] == mesh.endIndex(m, GridType::FULL)-1;
        } else if (gridStyle == HALF_LEAD) {
            res = indices[m][GridType::HALF] == mesh.startIndex(m, GridType::HALF) ||
                  indices[m][GridType::HALF] == mesh.endIndex(m, GridType::HALF)-1;
        }
    }
    return res;
} // atBounary

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
