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
protected:
    field<TimeLevels<field<T>, N>*> data;
    field<vector<StaggerType> > staggerTypes;
    FieldType fieldType;
    ArakawaGrid gridType;
public:
    StructuredField();
    virtual ~StructuredField();

    /**
     *  Create the memory storage for the scalar field.
     *
     *  @param fieldType    the field type.
     *  @param xStaggerType the stagger type of x grids.
     *  @param yStaggerType the stagger type of y grids.
     */
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, FieldType fieldType,
                StaggerType xStaggerType, StaggerType yStaggerType,
                bool hasHalfLevel = false);

    /**
     *  Create the memory storage for the scalar field.
     *
     *  @param fieldType    the field type.
     *  @param xStaggerType the stagger type of x grids.
     *  @param yStaggerType the stagger type of y grids.
     *  @param zStaggerType the stagger type of z grids.
     */
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, FieldType fieldType,
                StaggerType xStaggerType, StaggerType yStaggerType,
                StaggerType zStaggerType, bool hasHalfLevel = false);

    /**
     *  Create the memory storage for the vector field.
     *
     *  @param fieldType     the field type.
     *  @param xStaggerType0 the stagger type of x grids of the 1st component.
     *  @param yStaggerType0 the stagger type of y grids of the 1st component.
     *  @param xStaggerType1 the stagger type of x grids of the 2nd component.
     *  @param yStaggerType1 the stagger type of y grids of the 2nd component.
     */
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, FieldType fieldType,
                StaggerType xStaggerType0, StaggerType yStaggerType0,
                StaggerType xStaggerType1, StaggerType yStaggerType1,
                bool hasHalfLevel = false);

    /**
     *  Create the memory storage for the vector field.
     *
     *  @param fieldType     the field type.
     *  @param xStaggerType0 the stagger type of x grids of the 1st component.
     *  @param yStaggerType0 the stagger type of y grids of the 1st component.
     *  @param zStaggerType0 the stagger type of z grids of the 1st component.
     *  @param xStaggerType1 the stagger type of x grids of the 2nd component.
     *  @param yStaggerType1 the stagger type of y grids of the 2nd component.
     *  @param zStaggerType0 the stagger type of z grids of the 2nd component.
     *  @param xStaggerType2 the stagger type of x grids of the 3rd component.
     *  @param yStaggerType2 the stagger type of y grids of the 3rd component.
     *  @param zStaggerType0 the stagger type of z grids of the 3rd component.
     */
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, FieldType fieldType,
                StaggerType xStaggerType0, StaggerType yStaggerType0,
                StaggerType zStaggerType0, StaggerType xStaggerType1,
                StaggerType yStaggerType1, StaggerType zStaggerType1,
                StaggerType xStaggerType2, StaggerType yStaggerType2,
                StaggerType zStaggerType2, bool hasHalfLevel = false);

    /**
     *  Create the memory storage for the field.
     *
     *  @param fieldType the field type.
     *  @param numDim    the dimension number.
     *  @param gridType  the grid type (Arakawa grid type).
     */
    void create(const string &name, const string &units, const string &longName,
                const StructuredMesh &mesh, FieldType fieldType, int numDim,
                ArakawaGrid gridType, bool hasHalfLevel = false);

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
        for (int m = 0; m < data.size(); ++m) {
            int nx = data(m)->getLevel(0).n_rows;
            int ny = data(m)->getLevel(0).n_cols;
            int nz = data(m)->getLevel(0).n_slices;
            if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
                for (int k = 0; k < nz; ++k) {
                    for (int j = 0; j < ny; ++j) {
                        data(m)->getLevel(timeIdx)(0, j, k) =
                        data(m)->getLevel(timeIdx)(nx-2, j, k);
                        data(m)->getLevel(timeIdx)(nx-1, j, k) =
                        data(m)->getLevel(timeIdx)(1, j, k);
                    }
                }
            } else {
                REPORT_ERROR("Under construction!");
            }
            if (updateHalfLevel && data(m)->hasHalfLevel()) {
                TimeLevelIndex<2> halfTimeIdx = timeIdx-0.5;
                TimeLevelIndex<2> oldTimeIdx = timeIdx-1;
                for (int m = 0; m < data.size(); ++m) {
                    int nx = data(m)->getLevel(0).n_rows;
                    int ny = data(m)->getLevel(0).n_cols;
                    int nz = data(m)->getLevel(0).n_slices;
                    for (int k = 0; k < nz; ++k) {
                        for (int j = 0; j < ny; ++j) {
                            for (int i = 0; i < nx; ++i) {
                                data(m)->getLevel(halfTimeIdx)(i, j, k) =
                                (data(m)->getLevel(oldTimeIdx)(i, j, k)+
                                 data(m)->getLevel(timeIdx)(i, j, k))*0.5;
                            }
                        }
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
        for (int m = 0; m < data.size(); ++m) {
            int nx = data(m)->getLevel(0).n_rows;
            int ny = data(m)->getLevel(0).n_cols;
            int nz = data(m)->getLevel(0).n_slices;
            if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
                for (int k = 0; k < nz; ++k) {
                    for (int j = 0; j < ny; ++j) {
                        data(m)->getLevel(0)(0, j, k) =
                        data(m)->getLevel(0)(nx-2, j, k);
                        data(m)->getLevel(0)(nx-1, j, k) =
                        data(m)->getLevel(0)(1, j, k);
                    }
                }
            } else {
                REPORT_ERROR("Under construction!");
            }
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
        for (int i = 0; i < mesh->getTotalNumGrid(gridType); ++i) {
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
        for (int i = 0; i < mesh->getTotalNumGrid(gridType); ++i) {
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
    
    /**
     *  Subscript operator of the vector field.
     *
     *  @param comp    the component index.
     *  @param timeIdx the time level index.
     *  @param i       the index of x dimension.
     *  @param j       the index of y dimension.
     *  @param k       the index of z dimension.
     *
     *  @return The vector component on the given indices.
     */
    T operator()(int comp, const TimeLevelIndex<N> &timeIdx, int i, int j) const;
    T& operator()(int comp, const TimeLevelIndex<N> &timeIdx, int i, int j);
    T operator()(int comp, const TimeLevelIndex<N> &timeIdx, int i, int j, int k) const;
    T& operator()(int comp, const TimeLevelIndex<N> &timeIdx, int i, int j, int k);

    /**
     *  Subscript operator of the vector field by using one single index. The
     *  order is column-major. Note no virtual boundary grids will be accessed
     *  in this method.
     *
     *  @param comp     the component index.
     *  @param timeIdx  the time level index.
     *  @param cellIdx  the index of all dimensions.
     *
     *  @return The vector component on the given index.
     */
    T operator()(int comp, const TimeLevelIndex<N> &timeIdx, int cellIdx) const;
    T& operator()(int comp, const TimeLevelIndex<N> &timeIdx, int cellIdx);

    /**
     *  Get the stagger type on the given dimension of the scalar field.
     *
     *  @param dim the dimension index.
     *
     *  @return The stagger type on the given dimension.
     */
    inline StaggerType getStaggerType(int dim) const;
    
    /**
     *  Get the stagger type of the given component on the given dimension of
     *  the vector field.
     *
     *  @param comp the component index.
     *  @param dim  the dimension index.
     *
     *  @return The stagger type of the given component on the given dimension.
     */
    inline StaggerType getStaggerType(int comp, int dim) const;

    /**
     *  Convert the 2D vector field onto the given grid type.
     *
     *  @param gridType the grid type (Arakawa grid type).
     *  @param timeIdx  the time level index.
     *  @param xc        the x component.
     *  @param yc        the y component.
     */
    void convert(ArakawaGrid gridType, const TimeLevelIndex<2> &timeIdx,
                 mat &xc, mat &yc) const;

    /**
     *  Convert the 3D vector field onto the given grid type.
     *
     *  @param gridType the grid type (Arakawa grid type).
     *  @param timeIdx  the time level index.
     *  @param xc       the x component.
     *  @param yc       the y component.
     *  @param zc       the z component.
     */
    void convert(ArakawaGrid gridType, const TimeLevelIndex<2> &timeIdx,
                 cube &xc, cube &yc, cube &zc) const;
};

// -----------------------------------------------------------------------------

template <typename T, int N>
StructuredField<T, N>::StructuredField() : Field() {
}

template <typename T, int N>
StructuredField<T, N>::~StructuredField() {
    for (int i = 0; i < data.size(); ++i) {
        delete data(i);
    }
}

template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh,
                                   FieldType fieldType,
                                   StaggerType xStaggerType,
                                   StaggerType yStaggerType,
                                   bool hasHalfLevel) {
    if (fieldType != ScalarField) {
        REPORT_ERROR("Field should be a scalar!");
    }
    Field::common(name, units, longName, mesh, hasHalfLevel);
    this->fieldType = fieldType;
    staggerTypes.set_size(1);
    staggerTypes(0).resize(2);
    staggerTypes(0)[0] = xStaggerType;
    staggerTypes(0)[1] = yStaggerType;
    data.set_size(1);
    data(0) = new TimeLevels<field<T>, N>(hasHalfLevel);
    for (int i = 0; i < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
        data(0)->getLevel(i).set_size(mesh.getNumGrid(0, xStaggerType, true),
                                      mesh.getNumGrid(1, yStaggerType));
    }
    // set Arakawa grid stagger type for latter use
    if (xStaggerType == CENTER && yStaggerType == CENTER) {
        gridType = A_GRID;
    } else {
        REPORT_ERROR("Under construction!");
    }
}

template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh,
                                   FieldType fieldType,
                                   StaggerType xStaggerType,
                                   StaggerType yStaggerType,
                                   StaggerType zStaggerType,
                                   bool hasHalfLevel) {
    if (fieldType != ScalarField) {
        REPORT_ERROR("Field should be a scalar!")
    }
    Field::common(name, units, longName, mesh, hasHalfLevel);
    this->fieldType = fieldType;
    staggerTypes.set_size(1);
    staggerTypes(0).resize(3);
    staggerTypes(0)[0] = xStaggerType;
    staggerTypes(0)[1] = yStaggerType;
    staggerTypes(0)[2] = zStaggerType;
    data.set_size(1);
    data(0) = new TimeLevels<field<T>, N>(hasHalfLevel);
    for (int i = 0; i < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
        data(0)->getLevel(i).set_size(mesh.getNumGrid(0, xStaggerType, true),
                                      mesh.getNumGrid(1, yStaggerType),
                                      mesh.getNumGrid(2, zStaggerType));
    }
    // set Arakawa grid stagger type for latter use
    if (xStaggerType == CENTER && yStaggerType == CENTER &&
        zStaggerType == CENTER) {
        gridType = A_GRID;
    } else {
        REPORT_ERROR("Under construction!");
    }
}

template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh,
                                   FieldType fieldType,
                                   StaggerType xStaggerType0,
                                   StaggerType yStaggerType0,
                                   StaggerType xStaggerType1,
                                   StaggerType yStaggerType1,
                                   bool hasHalfLevel) {
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
    Field::common(name, units, longName, mesh, hasHalfLevel);
    this->fieldType = fieldType;
    staggerTypes.set_size(2);
    staggerTypes(0).resize(2);
    staggerTypes(1).resize(2);
    staggerTypes(0)[0] = xStaggerType0;
    staggerTypes(0)[1] = yStaggerType0;
    staggerTypes(1)[0] = xStaggerType1;
    staggerTypes(1)[1] = yStaggerType1;
    data.set_size(2);
    for (int m = 0; m < data.size(); ++m) {
        data(m) = new TimeLevels<field<T>, N>(hasHalfLevel);
        for (int i = 0; i < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
            data(m)->getLevel(i).set_size(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                          mesh.getNumGrid(1, staggerTypes[m][1]));
        }
    }
    // set Arakawa grid stagger type for latter use
    if (xStaggerType0 == CENTER && yStaggerType0 == CENTER &&
        xStaggerType1 == CENTER && yStaggerType1 == CENTER) {
        gridType = A_GRID;
    } else if (xStaggerType0 == EDGE   && yStaggerType0 == CENTER &&
               xStaggerType1 == CENTER && yStaggerType1 == EDGE) {
        gridType = C_GRID;
    } else {
        REPORT_ERROR("Under construction!");
    }
}

template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh,
                                   FieldType fieldType,
                                   StaggerType xStaggerType0,
                                   StaggerType yStaggerType0,
                                   StaggerType zStaggerType0,
                                   StaggerType xStaggerType1,
                                   StaggerType yStaggerType1,
                                   StaggerType zStaggerType1,
                                   StaggerType xStaggerType2,
                                   StaggerType yStaggerType2,
                                   StaggerType zStaggerType2,
                                   bool hasHalfLevel) {
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
    Field::common(name, units, longName, mesh, hasHalfLevel);
    this->fieldType = fieldType;
    staggerTypes.set_size(3);
    staggerTypes(0).resize(3);
    staggerTypes(1).resize(3);
    staggerTypes(2).resize(3);
    staggerTypes(0)[0] = xStaggerType0;
    staggerTypes(0)[1] = yStaggerType0;
    staggerTypes(0)[2] = zStaggerType0;
    staggerTypes(1)[0] = xStaggerType1;
    staggerTypes(1)[1] = yStaggerType1;
    staggerTypes(1)[2] = zStaggerType1;
    staggerTypes(2)[0] = xStaggerType2;
    staggerTypes(2)[1] = yStaggerType2;
    staggerTypes(2)[2] = zStaggerType2;
    data.set_size(3);
    for (int m = 0; m < data.size(); ++m) {
        data(m) = new TimeLevels<field<T>, N>(hasHalfLevel);
        for (int i = 0; i < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++i) {
            data(m)->getLevel(i).set_size(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                          mesh.getNumGrid(1, staggerTypes[m][1]),
                                          mesh.getNumGrid(2, staggerTypes[m][2]));
        }
    }
    // set Arakawa grid stagger type for latter use
    if (xStaggerType0 == CENTER && yStaggerType0 == CENTER && zStaggerType0 == CENTER &&
        xStaggerType1 == CENTER && yStaggerType1 == CENTER && zStaggerType1 == CENTER &&
        xStaggerType2 == CENTER && yStaggerType2 == CENTER && zStaggerType2 == CENTER) {
        gridType = A_GRID;
    } else if (xStaggerType0 == EDGE   && yStaggerType0 == CENTER && zStaggerType0 == CENTER &&
               xStaggerType1 == CENTER && yStaggerType1 == EDGE   && zStaggerType1 == CENTER &&
               xStaggerType2 == CENTER && yStaggerType2 == CENTER && zStaggerType2 == EDGE) {
        gridType = C_GRID;
    } else {
        REPORT_ERROR("Under construction!");
    }
}

template <typename T, int N>
void StructuredField<T, N>::create(const string &name, const string &units,
                                   const string &longName,
                                   const StructuredMesh &mesh,
                                   FieldType fieldType, int numDim,
                                   ArakawaGrid gridType, bool hasHalfLevel) {
    if (fieldType == ScalarField && gridType != A_GRID) {
        REPORT_ERROR("Scalar field should be on A grids!");
    }
    Field::common(name, units, longName, mesh, hasHalfLevel);
    this->gridType = gridType;
    switch (gridType) {
        case A_GRID:
            if (numDim == _2D) {
                if (fieldType == ScalarField) {
                    create(name, units, longName, mesh, fieldType,
                           CENTER, CENTER, hasHalfLevel);
                } else if (fieldType == VectorField) {
                    create(name, units, longName, mesh, fieldType,
                           CENTER, CENTER, CENTER, CENTER, hasHalfLevel);
                }
            } else if (numDim == _3D) {
                if (fieldType == ScalarField) {
                    create(name, units, longName, mesh, fieldType,
                           CENTER, CENTER, CENTER, hasHalfLevel);
                } else if (fieldType == VectorField) {
                    create(name, units, longName, mesh, fieldType,
                           CENTER, CENTER, CENTER, CENTER, CENTER, CENTER,
                           CENTER, CENTER, CENTER, hasHalfLevel);
                }
            }
            break;
        case B_GRID:
            REPORT_ERROR("Under construction!");
            break;
        case C_GRID:
            if (numDim == _2D) {
                create(name, units, longName, mesh, fieldType, EDGE, CENTER,
                       CENTER, EDGE, hasHalfLevel);
            } else if (numDim == _3D) {
                create(name, units, longName, mesh, fieldType, EDGE, CENTER,
                       CENTER, CENTER, EDGE, CENTER, CENTER, CENTER, EDGE,
                       hasHalfLevel);
            }
            break;
        case D_GRID:
            REPORT_ERROR("Under construction!");
            break;
        case E_GRID:
            REPORT_ERROR("Under construction!");
            break;
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
    return data(0)->getLevel(timeIdx)(I, J);
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
    return data(0)->getLevel(timeIdx)(I, J);
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
    return data(0)->getLevel(timeIdx)(I, J, k);
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
    return data(0)->getLevel(timeIdx)(I, J, k);
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
    return data(0)->getLevel(0)(I, J);
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
    return data(0)->getLevel(0)(I, J);
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
    return data(0)->getLevel(0)(I, J, k);
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
    return data(0)->getLevel(0)(I, J, k);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx) const {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, gridType);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data(0)->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(const TimeLevelIndex<N> &timeIdx, int cellIdx) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, gridType);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data(0)->getLevel(timeIdx)(i[0], i[1], i[2]);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(int cellIdx) const {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, gridType);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data(0)->getLevel(0)(i[0], i[1], i[2]);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int cellIdx) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    int i[3];
    mesh.unwrapIndex(cellIdx, i, gridType);
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            i[m] += 1;
        }
    }
    return data(0)->getLevel(0)(i[0], i[1], i[2]);
}
    
template <typename T, int N>
T StructuredField<T, N>::operator()(int comp, const TimeLevelIndex<N> &timeIdx,
                                 int i, int j) const {
#ifdef DEBUG
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
#endif
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
    return data(comp)->getLevel(timeIdx)(I, J);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int comp, const TimeLevelIndex<N> &timeIdx,
                                  int i, int j) {
#ifdef DEBUG
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
#endif
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
    return data(comp)->getLevel(timeIdx)(I, J);
}

template <typename T, int N>
T StructuredField<T, N>::operator()(int comp, const TimeLevelIndex<N> &timeIdx,
                                 int i, int j, int k) const {
#ifdef DEBUG
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
#endif
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
    return data(comp)->getLevel(timeIdx)(I, J, k);
}

template <typename T, int N>
T& StructuredField<T, N>::operator()(int comp, const TimeLevelIndex<N> &timeIdx,
                                  int i, int j, int k) {
#ifdef DEBUG
    if (fieldType != VectorField) {
        REPORT_ERROR("Field should be a vector!");
    }
#endif
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
    return data(comp)->getLevel(timeIdx)(I, J, k);
}

template <typename T, int N>
StaggerType StructuredField<T, N>::getStaggerType(int dim) const {
    if (fieldType != ScalarField) {
        REPORT_ERROR("Field is not a scalar!");
    }
    return staggerTypes(0)[dim];
}

template <typename T, int N>
StaggerType StructuredField<T, N>::getStaggerType(int comp, int dim) const {
    if (fieldType != VectorField) {
        REPORT_ERROR("Field is not a vector!");
    }
    return staggerTypes(comp)[dim];
}

template <typename T, int N>
void StructuredField<T, N>::convert(ArakawaGrid gridType,
                                    const TimeLevelIndex<2> &timeIdx,
                                    mat &xc, mat &yc) const {
    if (fieldType != VectorField) {
        REPORT_ERROR("Field is not a vector!");
    }
    if (data.size() != 2) {
        REPORT_ERROR("Field is not 2D!");
    }
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    if (this->gridType == C_GRID && gridType == A_GRID) {
        if (xc.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("Argument 'xc' dimension does not match!");
        } else if (xc.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("Argument 'xc' dimension does not match!");
        }
        if (yc.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("Argument 'yc' dimension does not match!");
        } else if (yc.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("Argument 'yc' dimension does not match!");
        }
        if (mesh.getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                    xc(i, j) = ((*this)(0, timeIdx, i-1, j)+
                                (*this)(0, timeIdx,   i, j))*0.5;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(1) == POLE) {
            for (int j = 1; j < mesh.getNumGrid(1, CENTER)-1; ++j) {
                for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                    yc(i, j) = ((*this)(1, timeIdx, i,   j)+
                                (*this)(1, timeIdx, i, j-1))*0.5;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}

template <typename T, int N>
void StructuredField<T, N>::convert(ArakawaGrid gridType,
                                    const TimeLevelIndex<2> &timeIdx,
                                    cube &xc, cube &yc, cube &zc) const {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    if (fieldType != VectorField) {
        REPORT_ERROR("Field is not a vector!");
    }
    if (data.size() != 3) {
        REPORT_ERROR("Field is not 3D!");
    }
    if (this->gridType == C_GRID && gridType == A_GRID) {
        if (xc.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("Argument 'xc' dimension does not match!");
        } else if (xc.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("Argument 'xc' dimension does not match!");
        } else if (xc.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("Argument 'xc' dimension does not match!");
        }
        if (yc.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("Argument 'yc' dimension does not match!");
        } else if (yc.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("Argument 'yc' dimension does not match!");
        } else if (yc.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("Argument 'yc' dimension does not match!");
        }
        if (zc.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("Argument 'zc' dimension does not match!");
        } else if (zc.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("Argument 'zc' dimension does not match!");
        } else if (zc.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("Argument 'zc' dimension does not match!");
        }
        if (mesh.getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
                for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        xc(i, j, k) = (data[0]->getLevel(timeIdx)(  i, j, k)+
                                       data[0]->getLevel(timeIdx)(i+1, j, k))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(1) == POLE) {
            for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
                for (int j = 1; j < mesh.getNumGrid(1, CENTER)-1; ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        yc(i, j, k) = (data[1]->getLevel(timeIdx)(i,   j, k)+
                                       data[1]->getLevel(timeIdx)(i, j-1, k))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(2) == RIGID) {
            for (int k = 1; k < mesh.getNumGrid(2, CENTER)-1; ++k) {
                for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        zc(i, j, k) = (data[2]->getLevel(timeIdx)(i, j, k)+
                                       data[2]->getLevel(timeIdx)(i, j, k-1))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}
    
}

#endif
