namespace geomtk {

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>::
StructuredField() : Field<MeshType>() {
    data = NULL;
}

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>::
~StructuredField() {
    if (data != NULL) {
        delete data;
    }
}

template <class MeshType, typename DataType, int NumTimeLevel>
void StructuredField<MeshType, DataType, NumTimeLevel>::
create(const string &name, const string &units, const string &longName,
       const MeshType &mesh, int loc, int numDim, bool hasHalfLevel) {
    Field<MeshType>::create(name, units, longName, mesh, numDim, hasHalfLevel);
    _staggerLocation = loc;
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
    for (int i = 0; i < data->numLevel(INCLUDE_HALF_LEVEL); ++i) {
        if (numDim == 2) {
            data->level(i).set_size(this->mesh().numGrid(0, gridTypes[0], true),
                                    this->mesh().numGrid(1, gridTypes[1], true),
                                    1);
        } else {
            data->level(i).set_size(this->mesh().numGrid(0, gridTypes[0], true),
                                    this->mesh().numGrid(1, gridTypes[1], true),
                                    this->mesh().numGrid(2, gridTypes[2], true));
        }
    }
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const field<DataType>& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx) const {
    return data->level(timeIdx);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
field<DataType>& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx) {
    return data->level(timeIdx);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const field<DataType>& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()() const {
    return data->level(0);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
field<DataType>& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()() {
    return data->level(0);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k) const {
    return data->level(timeIdx)(i, j, k);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k) {
    return data->level(timeIdx)(i, j, k);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int i, int j, int k) const {
    return data->level(0)(i, j, k);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int i, int j, int k) {
    return data->level(0)(i, j, k);
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) const {
    int i, j, k;
    switch (this->mesh().domain().numDim()) {
        case 1:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i);
            return data->level(timeIdx)(i);
        case 2:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j);
            return data->level(timeIdx)(i, j);
        case 3:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j, k);
            return data->level(timeIdx)(i, j, k);
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) {
    int i, j, k;
    switch (this->mesh().domain().numDim()) {
        case 1:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i);
            return data->level(timeIdx)(i);
        case 2:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j);
            return data->level(timeIdx)(i, j);
        case 3:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j, k);
            return data->level(timeIdx)(i, j, k);
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
const DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int cellIdx) const {
    int i, j, k;
    switch (this->mesh().domain().numDim()) {
        case 1:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i);
            return data->level(0)(i);
        case 2:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j);
            return data->level(0)(i, j);
        case 3:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j, k);
            return data->level(0)(i, j, k);
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
DataType& StructuredField<MeshType, DataType, NumTimeLevel>::
operator()(int cellIdx) {
    int i, j, k;
    switch (this->mesh().domain().numDim()) {
        case 1:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i);
            return data->level(0)(i);
        case 2:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j);
            return data->level(0)(i, j);
        case 3:
            this->mesh().unwrapIndex(staggerLocation(), cellIdx, i, j, k);
            return data->level(0)(i, j, k);
        default:
            REPORT_ERROR("Invalid dimension number!");
    }
} // operator()

template <class MeshType, typename DataType, int NumTimeLevel>
StructuredField<MeshType, DataType, NumTimeLevel>&
StructuredField<MeshType, DataType, NumTimeLevel>::
operator=(const StructuredField<MeshType, DataType, NumTimeLevel> &other) {
    if (this != &other) {
        Field<MeshType>::operator=(other);
        _staggerLocation = other._staggerLocation;
        gridTypes = other.gridTypes;
#ifndef NDEBUG
        assert(other.data != NULL);
#endif
        if (data == NULL) {
            data = new TimeLevels<field<DataType>, NumTimeLevel>(this->hasHalfLevel);
            for (int i = 0; i < data->numLevel(INCLUDE_HALF_LEVEL); ++i) {
                data->level(i).set_size(this->mesh().numGrid(0, gridTypes[0], true),
                                        this->mesh().numGrid(1, gridTypes[1], true),
                                        this->mesh().numGrid(2, gridTypes[2], true));
            }
        }
        for (int l = 0; l < other.data->numLevel(); ++l) {
            for (uword k = 0; k < other.data->level(l).n_slices; ++k) {
                for (uword j = 0; j < other.data->level(l).n_cols; ++j) {
                    for (uword i = 0; i < other.data->level(l).n_rows; ++i) {
                        data->level(l)(i, j, k) = other.data->level(l)(i, j, k);
                    }
                }
            }
        }
    }
    return *this;
} // operator=

} // geomtk
