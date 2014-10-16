#include "RLLDataFile.h"
#include "SphereDomain.h"
#include "ClassicPressureSigma.h"
#include "HybridPressureSigma.h"

namespace geomtk {

void RLLDataFile::inputGrids() {
    inputHorizontalGrids();
    inputVerticalGrids();
}

void RLLDataFile::inputHorizontalGrids() {
    SphereDomain &domain = mesh().domain();
    char units[30];
    size_t len;
    int varID, ret;
    for (int m = 0; m < 2; ++m) {
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileID, fullVarIDs[m], "units", units);
        CHECK_NC_GET_ATT(ret, fileName, domain.axisName(m), "units");
        if (m == 0) {
            assert(strcmp(units, "degree_east") == 0 ||
                   strcmp(units, "degrees_east") == 0);
        } else if (m == 1) {
            assert(strcmp(units, "degree_north") == 0 ||
                   strcmp(units, "degrees_north") == 0);
        }
        // get grid size along the axis
        ret = nc_inq_dimlen(fileID, fullDimIDs[m], &len);
        CHECK_NC_INQ_DIMLEN(ret, fileName, domain.axisName(m));
        vec full(len);
        ret = nc_get_var(fileID, fullVarIDs[m], full.memptr());
        CHECK_NC_GET_VAR(ret, fileName, domain.axisName(m));
        full *= RAD;
        if (bnds2D) {
            string name = domain.axisName(m)+"_bnds";
            ret = nc_inq_varid(fileID, name.c_str(), &varID);
            CHECK_NC_GET_VAR(ret, fileName, name);
            memset(units, 0, sizeof(units));
            ret = nc_get_att_text(fileID, varID, "units", units);
            CHECK_NC_GET_ATT(ret, fileName, name, "units");
            mat half2d(2, len);
            ret = nc_get_var(fileID, varID, half2d.memptr());
            CHECK_NC_GET_VAR(ret, fileName, name);
            vec half(len);
            for (int i = 0; i < len; ++i) {
                half[i] = half2d(1, i);
            }
            half *= RAD;
            mesh().setGridCoordComps(m, len, full, half);
        } else {
            mesh().setGridCoordComps(m, len, full);
        }
    }
}

void RLLDataFile::inputVerticalGrids() {
    SphereDomain &domain = mesh().domain();
    if (domain.numDim() != 3) {
        return;
    }
    char units[30];
    size_t len;
    int varID, ret;
    ret = nc_inq_dimlen(fileID, fullDimIDs[2], &len);
    CHECK_NC_INQ_DIMLEN(ret, fileName, domain.axisName(2));
    if (domain.vertCoord().type() == CLASSIC_PRESSURE_SIGMA) {
        ClassicPressureSigma &vertCoord = dynamic_cast<ClassicPressureSigma&>(domain.vertCoord());
        vertCoord.fullSigma.set_size(len);
        ret = nc_get_var(fileID, fullDimIDs[2], vertCoord.fullSigma.memptr());
        CHECK_NC_GET_VAR(ret, fileName, domain.axisName(2));
        assert(vertCoord.fullSigma.min() >= 0 && vertCoord.fullSigma.max() <= 1);
        vertCoord.halfSigma.set_size(len+1);
        ret = nc_inq_varid(fileID, "ilev", &varID);
        if (ret != NC_NOERR) {
            vertCoord.halfSigma[0] = 0;
            for (int k = 1; k < len; ++k) {
                vertCoord.halfSigma[k] = (vertCoord.fullSigma[k-1]+vertCoord.fullSigma[k])*0.5;
            }
            vertCoord.halfSigma[len] = 1;
            vertCoord.halfSigma.print();
            assert(vertCoord.halfSigma.min() >= 0 && vertCoord.halfSigma.max() <= 1);
        } else {
            ret = nc_get_var(fileID, varID, vertCoord.halfSigma.memptr());
            CHECK_NC_GET_VAR(ret, fileName, "ilev");
        }
        domain.setAxis(2, "lev", "classic sigma", "1", 0, RIGID, 1, RIGID);
        mesh().setGridCoordComps(2, len, vertCoord.fullSigma, vertCoord.halfSigma);
        ret = nc_inq_varid(fileID, "pmtop", &varID);
        CHECK_NC_INQ_VARID(ret, fileName, "pmtop");
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileID, varID, "units", units);
        CHECK_NC_GET_ATT(ret, fileName, "pmtop", "units");
        assert(strcmp(units, "Pa") == 0);
        ret = nc_get_var_double(fileID, varID, &vertCoord.pt);
        CHECK_NC_GET_VAR(ret, fileName, "pmtop");
    } else if (domain.vertCoord().type() == HYBRID_PRESSURE_SIGMA) {
        REPORT_ERROR("Under construction!");
    } else if (domain.vertCoord().type() == PRESSURE) {
        vec full(len);
        ret = nc_get_var(fileID, fullVarIDs[2], full.memptr());
        CHECK_NC_GET_VAR(ret, fileName, domain.axisName(2));
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileID, fullVarIDs[2], "units", units);
        CHECK_NC_GET_ATT(ret, fileName, domain.axisName(2), "units");
        if (strcmp(units, "hPa") == 0) {
            full *= 100;
        }
        domain.setAxis(2, "lev", "pressure levels", "Pa",
                       full[0], RIGID, full[len-1], RIGID);
        mesh().setGridCoordComps(2, len, full);
    } else {
        REPORT_ERROR("Under construction!");
    }
}

void RLLDataFile::outputGrids() {
    const auto &domain = mesh().domain();
    int ret;
    // write units
    ret = nc_redef(fileID);
    for (int m = 0; m < domain.numDim(); ++m) {
        string units = domain.axisUnits(m);
        if (m == 0) {
#ifndef NDEBUG
            assert(units == "radian_east");
#endif
            units = "degree_east";
        } else if (m == 1) {
#ifndef NDEBUG
            assert(units == "radian_north");
#endif
            units = "degree_north";
        }
        ret = nc_put_att(fileID, fullVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, fileName, domain.axisName(m), "units");
        ret = nc_put_att(fileID, halfVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, fileName, domain.axisName(m)+"_bnds", "units");
    }
    ret = nc_enddef(fileID);
    // write spatial grids
    for (int m = 0; m < domain.numDim(); ++m) {
        if (m == 0 || m == 1) {
            vec x = mesh().gridCoordComps(m, GridType::FULL)/RAD;
            ret = nc_put_var(fileID, fullVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileID, fullVarIDs[m],
                             mesh().gridCoordComps(m, GridType::FULL).memptr());
        }
        CHECK_NC_GET_VAR(ret, fileName, domain.axisName(m));
        if (m == 0 || m == 1) {
            vec x = mesh().gridCoordComps(m, GridType::HALF)/RAD;
            ret = nc_put_var(fileID, halfVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileID, halfVarIDs[m],
                             mesh().gridCoordComps(m, GridType::HALF).memptr());
        }
        CHECK_NC_GET_VAR(ret, fileName, domain.axisName(m)+"_bnds");
    }
}

} // geomtk