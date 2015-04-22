#include "CartesianMesh.h"
#include "CartesianMeshIndex.h"

namespace geomtk {

void CartesianMesh::
init(const string &filePath) {
    int ncId, varId, dimId, ret;
    nc_type xtype;
    int dimsp, dimIds[NC_MAX_VAR_DIMS];
    size_t numFullGrid, numHalfGrid;
    char str[100];
    ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
    CHECK_NC_OPEN(ret, filePath);
    for (int m = 0; m < domain().numDim(); ++m) {
        ret = nc_inq_varid(ncId, domain().axisName(m).c_str(), &varId);
        CHECK_NC_INQ_VARID(ret, filePath, domain().axisName(m));
        ret = nc_inq_vartype(ncId, varId, &xtype);
        CHECK_NC_INQ_VARTYPE(ret, filePath, domain().axisName(m));
        ret = nc_inq_varndims(ncId, varId, &dimsp);
        CHECK_NC_INQ_VARTYPE(ret, filePath, domain().axisName(m));
        assert(dimsp == 1);
        ret = nc_inq_vardimid(ncId, varId, dimIds);
        CHECK_NC_INQ_VARTYPE(ret, filePath, domain().axisName(m));
        ret = nc_inq_dimlen(ncId, dimIds[0], &numFullGrid);
        CHECK_NC_INQ_DIMLEN(ret, filePath, domain().axisName(m));
        vec full, half;
        full.set_size(numFullGrid);
        if (xtype == NC_FLOAT) {
            float *buffer = new float[numFullGrid];
            ret = nc_get_var(ncId, varId, buffer);
            CHECK_NC_GET_VAR(ret, filePath, domain().axisName(m));
            for (int i = 0; i < numFullGrid; ++i) {
                full[i] = buffer[i];
            }
            delete [] buffer;
        } else if (xtype == NC_DOUBLE) {
            double *buffer = new double[numFullGrid];
            ret = nc_get_var(ncId, varId, buffer);
            CHECK_NC_GET_VAR(ret, filePath, domain().axisName(m));
            for (int i = 0; i < numFullGrid; ++i) {
                full[i] = buffer[i];
            }
            delete [] buffer;
        }
        memset(&str[0], 0, sizeof(str));
        sprintf(str, "%s_bnds", domain().axisName(m).c_str());
        ret = nc_inq_dimid(ncId, str, &dimId);
        if (ret == NC_NOERR) {
            ret = nc_inq_dimlen(ncId, dimId, &numHalfGrid);
            CHECK_NC_INQ_DIMLEN(ret, filePath, str);
            ret = nc_inq_varid(ncId, str, &varId);
            CHECK_NC_INQ_VARID(ret, filePath, str);
            ret = nc_inq_vartype(ncId, varId, &xtype);
            CHECK_NC_INQ_VARTYPE(ret, filePath, str);
            half.set_size(numHalfGrid);
            if (xtype == NC_FLOAT) {
                float *buffer = new float[numHalfGrid];
                ret = nc_get_var(ncId, varId, buffer);
                CHECK_NC_GET_VAR(ret, filePath, str);
                for (int i = 0; i < numHalfGrid; ++i) {
                    half[i] = buffer[i];
                }
                delete [] buffer;
            } else if (xtype == NC_DOUBLE) {
                double *buffer = new double[numHalfGrid];
                ret = nc_get_var(ncId, varId, buffer);
                CHECK_NC_GET_VAR(ret, filePath, str);
                for (int i = 0; i < numHalfGrid; ++i) {
                    half[i] = buffer[i];
                }
                delete [] buffer;
            }
            setGridCoordComps(m, numFullGrid, full, half);
        } else {
            setGridCoordComps(m, numFullGrid, full);
        }
    }
    ret = nc_close(ncId);
    CHECK_NC_CLOSE(ret, filePath);
    StructuredMesh<CartesianDomain, SpaceCoord>::init(filePath);
} // init

void CartesianMesh::
init(int nx, int ny, int nz) {
    StructuredMesh<CartesianDomain, SpaceCoord>::init(nx, ny, nz);
} // init

void CartesianMesh::
setCellVolumes() {
    volumes.set_size(numGrid(0, GridType::FULL),
                     numGrid(1, GridType::FULL),
                     numGrid(2, GridType::FULL));
    if (domain().numDim() == 1) {
        for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
            int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
            double dx = gridInterval(0, GridType::HALF, I);
            volumes(I) = dx;
        }
    } else if (domain().numDim() == 2) {
        for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
            int J = gridStyles[1] == FULL_LEAD ? j-haloWidth() : j;
            double dy = gridInterval(1, GridType::HALF, J);
            for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
                double dx = gridInterval(0, GridType::HALF, I);
                volumes(I, J) = dx*dy;
            }
        }
    } else if (domain().numDim() == 3) {
        for (int k = ks(GridType::FULL); k <= ke(GridType::FULL); ++k) {
            int K = gridStyles[2] == FULL_LEAD ? k-haloWidth() : k;
            double dz = gridInterval(2, GridType::HALF, K);
            for (int j = js(GridType::FULL); j <= je(GridType::FULL); ++j) {
                int J = gridStyles[1] == FULL_LEAD ? j-haloWidth() : j;
                double dy = gridInterval(1, GridType::HALF, J);
                for (int i = is(GridType::FULL); i <= ie(GridType::FULL); ++i) {
                    int I = gridStyles[0] == FULL_LEAD ? i-haloWidth() : i;
                    double dx = gridInterval(0, GridType::HALF, I);
                    volumes(I, J, K) = dx*dy*dz;
                }
            }
        }
    }
#ifndef NDEBUG
    double totalVolume = 0.0, diff;
    for (int i = 0; i < totalNumGrid(Location::CENTER, domain().numDim()); ++i) {
        totalVolume += volumes(i);
    }
    if (domain().numDim() == 1) {
        diff = fabs(totalVolume-domain().axisSpan(0));
    } else if (domain().numDim() == 2) {
        diff = fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1));
    } else if (domain().numDim() == 3) {
        diff = fabs(totalVolume-domain().axisSpan(0)*domain().axisSpan(1)*domain().axisSpan(2));
    } else {
        REPORT_ERROR("Invalid dimension number!");
    }
    if (diff > 1.0e-10) {
        REPORT_ERROR("Total volume difference is " << diff << ", and it is too large!");
    }
#endif
} // setCellVolumes

void CartesianMesh::
move(const SpaceCoord &x0, double dt, const Velocity &v,
     const CartesianMeshIndex &idx, SpaceCoord &x1) const {
    x1() = x0()+dt*v();
    domain().isValid(x1);
} // move

} // geomtk