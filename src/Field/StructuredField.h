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
    int staggerLocation;
    vector<int> gridTypes;
public:
    StructuredField();
    virtual ~StructuredField();

    virtual void create(const string &name, const string &units, const string &longName,
                        const MeshType &mesh, int loc, int numDim, bool hasHalfLevel = false);

    DataType operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j) const;

    DataType& operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j);

    DataType operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k) const;

    DataType& operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int i, int j, int k);

    DataType operator()(int i, int j) const;

    DataType& operator()(int i, int j);

    DataType operator()(int i, int j, int k) const;

    DataType& operator()(int i, int j, int k);

    DataType operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx) const;

    DataType& operator()(const TimeLevelIndex<NumTimeLevel> &timeIdx, int cellIdx);

    DataType operator()(int cellIdx) const;

    DataType& operator()(int cellIdx);

    StructuredField<MeshType, DataType, NumTimeLevel>& operator=(const StructuredField<MeshType, DataType, NumTimeLevel> &other);

    virtual int getStaggerLocation() const { return staggerLocation; }

    virtual int getGridType(int axisIdx) const { return gridTypes[axisIdx]; }

    template <typename Q = DataType>
    typename enable_if<has_operator_plus<Q>::value || is_arithmetic<Q>::value, void>::type
    applyBndCond(const TimeLevelIndex<NumTimeLevel> &timeIdx, bool updateHalfLevel = false) {
        int nx = data->getLevel(0).n_rows;
        int ny = data->getLevel(0).n_cols;
        int nz = data->getLevel(0).n_slices;
        if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
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
            if (NumTimeLevel < 2) {
                REPORT_ERROR("Time level (" << NumTimeLevel << ") is less than 2, " <<
                             "so there is no half time level!");
            }
            TimeLevelIndex<NumTimeLevel> halfTimeIdx = timeIdx-0.5;
            TimeLevelIndex<NumTimeLevel> oldTimeIdx = timeIdx-1;
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

    template <typename Q = DataType>
    typename enable_if<has_operator_plus<Q>::value || is_arithmetic<Q>::value, void>::type
    applyBndCond() {
        int nx = data->getLevel(0).n_rows;
        int ny = data->getLevel(0).n_cols;
        int nz = data->getLevel(0).n_slices;
        if (this->mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
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

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, void>::type
    max(const TimeLevelIndex<NumTimeLevel> &timeIdx) {
        DataType res = -999999;
        for (int i = 0; i < this->mesh->getTotalNumGrid(staggerLocation, this->getNumDim()); ++i) {
            if (res < (*this)(timeIdx, i)) {
                res = (*this)(timeIdx, i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, void>::type
    max() {
        DataType res = -999999;
        for (int i = 0; i < this->mesh->getTotalNumGrid(staggerLocation, this->getNumDim()); ++i) {
            if (res < (*this)(0, i)) {
                res = (*this)(0, i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, void>::type
    min(const TimeLevelIndex<NumTimeLevel> &timeIdx) {
        DataType res = 999999;
        for (int i = 0; i < this->mesh->getTotalNumGrid(staggerLocation, this->getNumDim()); ++i) {
            if (res > (*this)(timeIdx, i)) {
                res = (*this)(timeIdx, i);
            }
        }
        return res;
    }

    template <typename Q = DataType>
    typename enable_if<is_arithmetic<Q>::value, void>::type
    min() {
        DataType res = 999999;
        for (int i = 0; i < this->mesh->getTotalNumGrid(staggerLocation, this->getNumDim()); ++i) {
            if (res > (*this)(0, i)) {
                res = (*this)(0, i);
            }
        }
        return res;
    }
};

} // geomtk

#include "StructuredField-impl.h"

#endif // __Geomtk_StructuredField__
