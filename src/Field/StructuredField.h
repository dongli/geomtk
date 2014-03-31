#ifndef __Geomtk_StructuredField__
#define __Geomtk_StructuredField__

#include "Field.h"
#include "StructuredMesh.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on structured mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <typename T, int N>
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
    
    StructuredField<T, N>& operator=(const StructuredField<T, N> &other);

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

template <typename T, int N>
StructuredField<T, N>& StructuredField<T, N>::operator=(const StructuredField<T, N> &other) {
    if (this != &other) {
        Field::operator=(other);
        staggerLocation = other.staggerLocation;
        gridTypes = other.gridTypes;
        assert(other.data != NULL);
        if (data == NULL) {
            const StructuredMesh *mesh = static_cast<const StructuredMesh*>(this->mesh);
            data = new TimeLevels<field<T>, N>(hasHalfLevel);
            for (int i = 0; i < data->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
                data->getLevel(i).set_size(mesh->getNumGrid(0, gridTypes[0], true),
                                           mesh->getNumGrid(1, gridTypes[1], true),
                                           mesh->getNumGrid(2, gridTypes[2], true));
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

}

#endif
