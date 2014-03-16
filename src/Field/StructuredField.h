#ifndef __Geomtk_StructuredField__
#define __Geomtk_StructuredField__

#include "Field.h"
#include "StructuredMesh.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on structured mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <typename T, int N = 2>
class StructuredField : public Field {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    TimeLevels<field<T>, N> *data;
    int staggerLocation;
    vector<int> gridTypes;
public:
    StructuredField();
    virtual ~StructuredField();
    
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, int loc, bool hasHalfLevel = false);

    /**
     *  Apply boundary conditions after the scalar field is updated.
     *  This function is only valid when T can be added and carried on
     *  arithmetic operations.
     *
     *  @param timeIdx       the time level index.
     *  @param updateHalfLevel the flag for updating half level.
     */
    template <typename Q = T>
    typename enable_if<has_operator_plus<Q>::value ||
                       is_arithmetic<Q>::value, void>::type
    applyBndCond(const TimeLevelIndex<N> &timeIdx,
                      bool updateHalfLevel = false) {
        int nx = data->getLevel(0).n_rows;
        int ny = data->getLevel(0).n_cols;
        int nz = data->getLevel(0).n_slices;
        if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    data->getLevel(timeIdx)(0, j, k) =
                    data->getLevel(timeIdx)(nx-2, j, k);
                    data->getLevel(timeIdx)(nx-1, j, k) =
                    data->getLevel(timeIdx)(1, j, k);
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (updateHalfLevel && data->hasHalfLevel()) {
            TimeLevelIndex<2> halfTimeIdx = timeIdx-0.5;
            TimeLevelIndex<2> oldTimeIdx = timeIdx-1;
            int nx = data->getLevel(0).n_rows;
            int ny = data->getLevel(0).n_cols;
            int nz = data->getLevel(0).n_slices;
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < nx; ++i) {
                        data->getLevel(halfTimeIdx)(i, j, k) =
                            (data->getLevel(oldTimeIdx)(i, j, k)+
                             data->getLevel(timeIdx   )(i, j, k))*0.5;
                    }
                }
            }
        }
    }
    
    /**
     *  Apply boundary conditions after the scalar field is updated.
     *  This function is only valid when T can be added and carried on
     *  arithmetic operations.
     */
    template <typename Q = T>
    typename enable_if<has_operator_plus<Q>::value ||
                       is_arithmetic<Q>::value, void>::type
    applyBndCond() {
        int nx = data->getLevel(0).n_rows;
        int ny = data->getLevel(0).n_cols;
        int nz = data->getLevel(0).n_slices;
        if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    data->getLevel(0)(0, j, k) =
                    data->getLevel(0)(nx-2, j, k);
                    data->getLevel(0)(nx-1, j, k) =
                    data->getLevel(0)(1, j, k);
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }

    /**
     *  Get the maximum value of the field (only for arithmetic field).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The maximum value.
     */
    template <typename Q = T>
    typename enable_if<is_arithmetic<Q>::value, T>::type
    max(const TimeLevelIndex<N> &timeIdx) {
        T res = -999999;
        for (int i = 0; i < mesh->getTotalNumGrid(staggerLocation); ++i) {
            if (res < (*this)(timeIdx, i)) {
                res = (*this)(timeIdx, i);
            }
        }
        return res;
    }

    /**
     *  Get the maximum value of the field (only for arithmetic field).
     *
     *  @return The maximum value.
     */
    template <typename Q = T>
    typename enable_if<is_arithmetic<Q>::value, T>::type
    max() {
        T res = -999999;
        for (int i = 0; i < mesh->getTotalNumGrid(staggerLocation); ++i) {
            if (res < (*this)(0, i)) {
                res = (*this)(0, i);
            }
        }
        return res;
    }
    
    /**
     *  Subscript operator of the scalar field.
     *
     *  @param timeIdx the time level index.
     *  @param i       the index of x dimension.
     *  @param j       the index of y dimension.
     *  @param k       the index of z dimension.
     *
     *  @return The scalar on the given indices.
     */
    T operator()(const TimeLevelIndex<N> &timeIdx, int i, int j) const;
    T& operator()(const TimeLevelIndex<N> &timeIdx, int i, int j);
    T operator()(const TimeLevelIndex<N> &timeIdx, int i, int j, int k) const;
    T& operator()(const TimeLevelIndex<N> &timeIdx, int i, int j, int k);

    T operator()(int i, int j) const;
    T& operator()(int i, int j);
    T operator()(int i, int j, int k) const;
    T& operator()(int i, int j, int k);

    /**
     *  Subscript operator of the scalar field by using one single index. The
     *  order is column-major. Note no virtual boundary grids will be accessed
     *  in this method.
     *
     *  @param timeIdx  the time level index.
     *  @param cellIdx  the index of all dimensions.
     *
     *  @return The scalar on the given index.
     */
    T operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx) const;
    T& operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx);

    T operator()(int cellIdx) const;
    T& operator()(int cellIdx);

    virtual int getStaggerLocation() const { return staggerLocation; }

    virtual int getGridType(int axisIdx) const { return gridTypes[axisIdx]; }
};

// -----------------------------------------------------------------------------

template <typename T, int N>
StructuredField<T, N>::StructuredField() : Field() {
    data = NULL;
}

template <typename T, int N>
StructuredField<T, N>::~StructuredField() {
    if (data != NULL) {
        delete data;
    }
}
    
template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh, int loc,
                                   bool hasHalfLevel) {
    Field::common(name, units, longName, mesh, hasHalfLevel);
    staggerLocation = loc;
    gridTypes.resize(3);
    switch (loc) {
        case Location::CENTER:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::FULL;
            gridTypes[2] = GridType::FULL;
            break;
        case Location::X_FACE:
            gridTypes[0] = GridType::HALF;
            gridTypes[1] = GridType::FULL;
            gridTypes[2] = GridType::FULL;
            break;
        case Location::Y_FACE:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::HALF;
            gridTypes[2] = GridType::FULL;
            break;
        case Location::Z_FACE:
            gridTypes[0] = GridType::FULL;
            gridTypes[1] = GridType::FULL;
            gridTypes[2] = GridType::HALF;
            break;
        case Location::XY_VERTEX:
            gridTypes[0] = GridType::HALF;
            gridTypes[1] = GridType::HALF;
            gridTypes[2] = GridType::FULL;
            break;
        default:
            REPORT_ERROR("Unsupported stagger location!");
            break;
    }
    data = new TimeLevels<field<T>, N>(hasHalfLevel);
    for (int i = 0; i < data->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
        data->getLevel(i).set_size(mesh.getNumGrid(0, gridTypes[0], true),
                                   mesh.getNumGrid(1, gridTypes[1], true),
                                   mesh.getNumGrid(2, gridTypes[2], true));
    }
}

template <typename T, int N>
T StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx,
                                 int i, int j) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx,
                                  int i, int j) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx,
                                 int i, int j, int k) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J, k);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx,
                                  int i, int j, int k) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(timeIdx)(I, J, k);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(int i, int j) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int i, int j) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(int i, int j, int k) const {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J, k);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int i, int j, int k) {
    // The virtual boundary grids at the periodic boundary conditions are hiden
    // from user.
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data->getLevel(0)(I, J, k);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx) const {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(int cellIdx) const {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(0)(i[0], i[1], i[2]);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int cellIdx) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, staggerLocation);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data->getLevel(0)(i[0], i[1], i[2]);
}
    
}

#endif
