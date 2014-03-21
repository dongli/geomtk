#ifndef __Geomtk_NumericStructuredField__
#define __Geomtk_NumericStructuredField__

#include "StructuredField.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on structured mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <typename T, int N>
class NumericStructuredField : public StructuredField<T, N> {
protected:
    using StructuredField<T, N>::data;
    using StructuredField<T, N>::mesh;
    using StructuredField<T, N>::staggerLocation;
public:
    NumericStructuredField();
    virtual ~NumericStructuredField();
    
    /**
     *  Apply boundary conditions after the scalar field is updated.
     *  This function is only valid when T can be added and carried on
     *  arithmetic operations.
     *
     *  @param timeIdx       the time level index.
     *  @param updateHalfLevel the flag for updating half level.
     */
    virtual void applyBndCond(const TimeLevelIndex<N> &timeIdx,
                              bool updateHalfLevel = false);
    
    /**
     *  Apply boundary conditions after the scalar field is updated.
     *  This function is only valid when T can be added and carried on
     *  arithmetic operations.
     */
    virtual void applyBndCond();

    /**
     *  Get the maximum value of the field (only for arithmetic field).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The maximum value.
     */
    virtual T max(const TimeLevelIndex<N> &timeIdx) const;

    /**
     *  Get the maximum value of the field (only for arithmetic field).
     *
     *  @return The maximum value.
     */
    virtual T max() const;
};

// -----------------------------------------------------------------------------

template <typename T, int N>
NumericStructuredField<T, N>::NumericStructuredField() : StructuredField<T, N>() {
}

template <typename T, int N>
NumericStructuredField<T, N>::~NumericStructuredField() {
}
 
template <typename T, int N>
void NumericStructuredField<T, N>::applyBndCond(const TimeLevelIndex<N> &timeIdx,
                                                bool updateHalfLevel) {
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
        if (N < 2) {
            REPORT_ERROR("Time level (" << N << ") is less than 2, " <<
                         "so there is no half time level!");
        }
        TimeLevelIndex<N> halfTimeIdx = timeIdx-0.5;
        TimeLevelIndex<N> oldTimeIdx = timeIdx-1;
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

template <typename T, int N>
void NumericStructuredField<T, N>::applyBndCond() {
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

template <typename T, int N>
T NumericStructuredField<T, N>::max(const TimeLevelIndex<N> &timeIdx) const {
    T res = -999999;
    for (int i = 0; i < mesh->getTotalNumGrid(staggerLocation); ++i) {
        if (res < (*this)(timeIdx, i)) {
            res = (*this)(timeIdx, i);
        }
    }
    return res;
}

template <typename T, int N>
T NumericStructuredField<T, N>::max() const {
    T res = -999999;
    for (int i = 0; i < mesh->getTotalNumGrid(staggerLocation); ++i) {
        if (res < (*this)(0, i)) {
            res = (*this)(0, i);
        }
    }
    return res;
}

}

#endif // __Geomtk_NumericStructuredField__
