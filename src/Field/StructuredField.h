#ifndef __Geomtk_StructuredField__
#define __Geomtk_StructuredField__

#include "Field.h"
#include "StructuredMesh.h"

namespace geomtk {

/**
 *  This class specifies the scalar field on structured mesh. The data type is
 *  templated, so any proper basic type (e.g. double) and classes can be used.
 */
template <class MeshType, typename DataType, int NumTimeLevel>
class StructuredField : public Field<MeshType> {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
protected:
    TimeLevels<field<DataType>, NumTimeLevel> *data;
    int _staggerLocation;
    vector<int> gridTypes;
public:
    StructuredField();
    virtual ~StructuredField();

    virtual void
    create(const string &name, const string &units, const string &longName,
           const MeshType &mesh, int loc, int numDim, bool hasHalfLevel = false);

    const field<DataType>&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx) const;

    field<DataType>&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx);

    const field<DataType>&
    operator()() const;

    field<DataType>&
    operator()();

    const DataType&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k = 0) const;

    DataType&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k = 0);

    const DataType& operator()(int i, int j, int k = 0) const;

    DataType& operator()(int i, int j, int k = 0);

    const DataType&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) const;

    DataType&
    operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx);

    const DataType& operator()(int i) const;

    DataType& operator()(int i);

    StructuredField<MeshType, DataType, NumTimeLevel>&
    operator=(const StructuredField<MeshType, DataType, NumTimeLevel> &other);

    virtual int staggerLocation() const { return _staggerLocation; }

    virtual int gridType(int axisIdx) const { return gridTypes[axisIdx]; }

    template <typename Q = DataType>
    typename enable_if<has_operator_plus<Q>::value || is_arithmetic<Q>::value, void>::type
    applyBndCond(const TimeLevelIndex<NumTimeLevel> &timeIdx, bool updateHalfLevel = false) {
        int nx = data->level(0).n_rows;
        int ny = data->level(0).n_cols;
        int nz = data->level(0).n_slices;
        const auto &domain = this->mesh().domain();
        field<DataType> &d = data->level(timeIdx);
        if (domain.axisStartBndType(0) == PERIODIC) {
            // TODO: Need to modify when doing parallel.
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < this->mesh().haloWidth(); ++i) {
                        d(i, j, k) = d(this->mesh().ie(gridType(0))-this->mesh().haloWidth()+1+i, j, k);
                        d(this->mesh().ie(gridType(0))+1+i, j, k) = d(this->mesh().is(gridType(0))+i, j, k);
                    }
                }
            }
        }
        if (domain.axisStartBndType(1) == PERIODIC) {
            for (int k = 0; k < nz; ++k) {
                for (int i = 0; i < nx; ++i) {
                    for (int j = 0; j < this->mesh().haloWidth(); ++j) {
                        d(i, j, k) = d(i, this->mesh().ie(gridType(1))-this->mesh().haloWidth()+1+j, k);
                        d(i, this->mesh().ie(gridType(1))+1+j, k) = d(i, this->mesh().is(gridType(1))+j, k);
                    }
                }
            }
        }
        if (domain.numDim() == 3) {
            if (domain.axisStartBndType(2) == PERIODIC) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < nx; ++i) {
                        for (int k = 0; k < this->mesh().haloWidth(); ++k) {
                            d(i, j, k) = d(i, j, this->mesh().ie(gridType(2))-this->mesh().haloWidth()+1+k);
                            d(i, j, this->mesh().ie(gridType(2))+1+k) = d(i, j, this->mesh().is(gridType(2))+k);
                        }
                    }
                }
            }
        }
        if (updateHalfLevel && data->hasHalfLevel()) {
            if (NumTimeLevel < 2) {
                REPORT_ERROR("Time level (" << NumTimeLevel << ") is less than 2, " <<
                             "so there is no half time level!");
            }
            TimeLevelIndex<NumTimeLevel> halfTimeIdx = timeIdx-0.5;
            TimeLevelIndex<NumTimeLevel> oldTimeIdx = timeIdx-1;
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < nx; ++i) {
                        data->level(halfTimeIdx)(i, j, k) =
                            (data->level(oldTimeIdx)(i, j, k)+
                             data->level(timeIdx   )(i, j, k))*0.5;
                    }
                }
            }
        }
    }

    template <typename Q = DataType>
    typename enable_if<has_operator_plus<Q>::value || is_arithmetic<Q>::value, void>::type
    applyBndCond() {
        int nx = data->level(0).n_rows;
        int ny = data->level(0).n_cols;
        int nz = data->level(0).n_slices;
        const auto &domain = this->mesh().domain();
        field<DataType> &d = data->level(0);
        if (domain.axisStartBndType(0) == PERIODIC) {
            // TODO: Need to modify when doing parallel.
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < this->mesh().haloWidth(); ++i) {
                        d(i, j, k) = d(this->mesh().ie(gridType(0))-this->mesh().haloWidth()+1+i, j, k);
                        d(this->mesh().ie(gridType(0))+1+i, j, k) = d(this->mesh().is(gridType(0))+i, j, k);
                    }
                }
            }
        }
        if (domain.axisStartBndType(1) == PERIODIC) {
            for (int k = 0; k < nz; ++k) {
                for (int i = 0; i < nx; ++i) {
                    for (int j = 0; j < this->mesh().haloWidth(); ++j) {
                        d(i, j, k) = d(i, this->mesh().ie(gridType(1))-this->mesh().haloWidth()+1+j, k);
                        d(i, this->mesh().ie(gridType(1))+1+j, k) = d(i, this->mesh().is(gridType(1))+j, k);
                    }
                }
            }
        }
        if (domain.numDim() == 3) {
            if (domain.axisStartBndType(2) == PERIODIC) {
                for (int j = 0; j < ny; ++j) {
                    for (int i = 0; i < nx; ++i) {
                        for (int k = 0; k < this->mesh().haloWidth(); ++k) {
                            d(i, j, k) = d(i, j, this->mesh().ie(gridType(2))-this->mesh().haloWidth()+1+k);
                            d(i, j, this->mesh().ie(gridType(2))+1+k) = d(i, j, this->mesh().is(gridType(2))+k);
                        }
                    }
                }
            }
        }
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    max(const TimeLevelIndex<NumTimeLevel> &timeIdx) const {
        DataType res = -999999;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (res < (*this)(timeIdx, i)) {
                res = (*this)(timeIdx, i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    max() const {
        DataType res = -999999;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (res < (*this)(i)) {
                res = (*this)(i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    min(const TimeLevelIndex<NumTimeLevel> &timeIdx) const {
        DataType res = 999999;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (res > (*this)(timeIdx, i)) {
                res = (*this)(timeIdx, i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    min() const {
        DataType res = 999999;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (res > (*this)(i)) {
                res = (*this)(i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    sum(const TimeLevelIndex<NumTimeLevel> &timeIdx) const {
        DataType res = 0;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            res += (*this)(timeIdx, i);
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, DataType>::type
    sum() const {
        DataType res = 0;
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            res += (*this)(i);
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, bool>::type
    hasNan(const TimeLevelIndex<NumTimeLevel> &timeIdx) const {
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (std::isnan((*this)(timeIdx, i))) {
                return true;
            }
        }
        return false;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, bool>::type
    hasNan() const {
        for (int i = 0; i < this->mesh().totalNumGrid(staggerLocation(), this->numDim()); ++i) {
            if (std::isnan((*this)(i))) {
                return true;
            }
        }
        return false;
    }
};

} // geomtk

#include "StructuredField-impl.h"

#endif // __Geomtk_StructuredField__
