namespace geomtk {

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>::StructuredField() : Field<MeshType>() {
    data = NULL;
}

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>::~StructuredField() {
    if (data != NULL) {
        delete data;
    }
}

template <class MeshType, typename DataType, int NumTimeLevel>
void StructuredField<MeshType, DataType, NumTimeLevel>::
create(const string &name, const string &units, const string &longName,
       const MeshType &mesh, int loc, int numDim, bool hasHalfLevel) {
    Field<MeshType>::create(name, units, longName, mesh, numDim, hasHalfLevel);
    staggerLocation = loc;
    gridTypes.resize(numDim);
    switch (loc) {
        case Location::CENTER:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::FULL;
            if (numDim == 3) gridTypes[2] = GridType::FULL;
            break;
        case Location::X_FACE:
            gridTypes[0] = GridType::HALF;
            gridTypes[1] = GridType::FULL;
            if (numDim == 3) gridTypes[2] = GridType::FULL;
            break;
        case Location::Y_FACE:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::HALF;
            if (numDim == 3) gridTypes[2] = GridType::FULL;
            break;
        case Location::Z_FACE:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::FULL;
            if (numDim == 3) gridTypes[2] = GridType::HALF;
            break;
        case Location::XY_VERTEX:
            gridTypes[0] = GridType::HALF;
            gridTypes[1] = GridType::HALF;
            if (numDim == 3) gridTypes[2] = GridType::FULL;
            break;
        default:
            REPORT_ERROR("Unsupported stagger location!");
            break;
    }
    data = new TimeLevels<field<DataType>, NumTimeLevel>(hasHalfLevel);
    for (int i = 0; i < data->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
        if (numDim == 2) {
            data->getLevel(i).set_size(this->mesh->getNumGrid(0, gridTypes[0], true),
                                       this->mesh->getNumGrid(1, gridTypes[1], true),
                                       1);
        } else {
            data->getLevel(i).set_size(this->mesh->getNumGrid(0, gridTypes[0], true),
                                       this->mesh->getNumGrid(1, gridTypes[1], true),
                                       this->mesh->getNumGrid(2, gridTypes[2], true));
        }
    }
}

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (this->mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J, k);
}

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (this->mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J, k);
}

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int i, int j, int k) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (this->mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J, k);
}

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int i, int j, int k) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (this->mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J, k);
}

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) const {
    int i[3];
    this->mesh->unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < this->mesh->getDomain().getNumDim(); ++m) {
        if (this->mesh->getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) {
    int i[3];
    this->mesh->unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < this->mesh->getDomain().getNumDim(); ++m) {
        if (this->mesh->getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int cellIdx) const {
    int i[3];
    this->mesh->unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < this->mesh->getDomain().getNumDim(); ++m) {
        if (this->mesh->getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(0)(i[0], i[1], i[2]);
}

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int cellIdx) {
    int i[3];
    this->mesh->unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < this->mesh->getDomain().getNumDim(); ++m) {
        if (this->mesh->getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(0)(i[0], i[1], i[2]);
}

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>&
StructuredField<MeshType, DataType, NumTimeLevel>::
operator=(const StructuredField<MeshType, DataType, NumTimeLevel> &other) {
    if (this != &other) {
        Field<MeshType>::operator=(other);
        staggerLocation = other.staggerLocation;
        gridTypes = other.gridTypes;
#ifndef NDEBUG
        assert(other.data != NULL);
#endif
        if (data == NULL) {
            data = new TimeLevels<field<DataType>, NumTimeLevel>(this->hasHalfLevel);
            for (int i = 0; i < data->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
                data->getLevel(i).set_size(this->mesh->getNumGrid(0, gridTypes[0], true),
                                           this->mesh->getNumGrid(1, gridTypes[1], true),
                                           this->mesh->getNumGrid(2, gridTypes[2], true));
            }
        }
        for (int l = 0; l < other.data->getNumLevel(); ++l) {
            for (int k = 0; k < other.data->getLevel(l).n_slices; ++k) {
                for (int j = 0; j < other.data->getLevel(l).n_cols; ++j) {
                    for (int i = 0; i < other.data->getLevel(l).n_rows; ++i) {
                        data->getLevel(l)(i, j, k) = other.data->getLevel(l)(i, j, k);
                    }
                }
            }
        }
    }
    return *this;
}

} // geomtk
