#include "RLLDataFile.h"
#include "SphereDomain.h"
#include "ClassicPressureSigma.h"
#include "HybridPressureSigma.h"

namespace geomtk {

void RLLDataFile::
inputHorizontalMesh() {
    SphereDomain &domain = mesh().domain();
    char units[30];
    size_t len;
    int varId, ret;
    for (int m = 0; m < 2; ++m) {
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileId, fullVarIDs[m], "units", units);
        CHECK_NC_GET_ATT(ret, filePath, domain.axisName(m), "units");
        if (m == 0) {
            assert(strcmp(units, "degree_east") == 0 ||
                   strcmp(units, "degrees_east") == 0);
        } else if (m == 1) {
            assert(strcmp(units, "degree_north") == 0 ||
                   strcmp(units, "degrees_north") == 0);
        }
        // get grid size along the axis
        ret = nc_inq_dimlen(fileId, fullDimIDs[m], &len);
        CHECK_NC_INQ_DIMLEN(ret, filePath, domain.axisName(m));
        vec full(len);
        ret = nc_get_var(fileId, fullVarIDs[m], full.memptr());
        CHECK_NC_GET_VAR(ret, filePath, domain.axisName(m));
        full *= RAD;
        if (bnds2D) {
            string name = domain.axisName(m)+"_bnds";
            ret = nc_inq_varid(fileId, name.c_str(), &varId);
            CHECK_NC_GET_VAR(ret, filePath, name);
            memset(units, 0, sizeof(units));
            ret = nc_get_att_text(fileId, varId, "units", units);
            CHECK_NC_GET_ATT(ret, filePath, name, "units");
            mat half2d(2, len);
            ret = nc_get_var(fileId, varId, half2d.memptr());
            CHECK_NC_GET_VAR(ret, filePath, name);
            vec half(len);
            for (size_t i = 0; i < len; ++i) {
                half[i] = half2d(1, i);
            }
            half *= RAD;
            mesh().setGridCoordComps(m, len, full, half);
        } else {
            mesh().setGridCoordComps(m, len, full);
        }
    }
} // inputHorizontalMesh

void RLLDataFile::
inputVerticalMesh() {
    SphereDomain &domain = mesh().domain();
    if (domain.numDim() != 3) {
        return;
    }
    char units[30];
    size_t len;
    int varId, ret;
    ret = nc_inq_dimlen(fileId, fullDimIDs[2], &len);
    CHECK_NC_INQ_DIMLEN(ret, filePath, domain.axisName(2));
    if (domain.vertCoord().type() == CLASSIC_PRESSURE_SIGMA) {
        ClassicPressureSigma &vertCoord = dynamic_cast<ClassicPressureSigma&>(domain.vertCoord());
        vertCoord.sigma[VertCoord::GridType::FULL].set_size(len);
        ret = nc_get_var(fileId, fullDimIDs[2], vertCoord.sigma[VertCoord::GridType::FULL].memptr());
        CHECK_NC_GET_VAR(ret, filePath, domain.axisName(2));
        assert(vertCoord.sigma[VertCoord::GridType::FULL].min() >= 0 && vertCoord.sigma[VertCoord::GridType::FULL].max() <= 1);
        vertCoord.sigma[VertCoord::GridType::HALF].set_size(len+1);
        ret = nc_inq_varid(fileId, "ilev", &varId);
        if (ret != NC_NOERR) {
            vertCoord.sigma[VertCoord::GridType::HALF][0] = 0;
            for (size_t k = 1; k < len; ++k) {
                vertCoord.sigma[VertCoord::GridType::HALF][k] = (vertCoord.sigma[VertCoord::GridType::FULL][k-1]+vertCoord.sigma[VertCoord::GridType::FULL][k])*0.5;
            }
            vertCoord.sigma[VertCoord::GridType::HALF][len] = 1;
            vertCoord.sigma[VertCoord::GridType::HALF].print();
            assert(vertCoord.sigma[VertCoord::GridType::HALF].min() >= 0 && vertCoord.sigma[VertCoord::GridType::HALF].max() <= 1);
        } else {
            ret = nc_get_var(fileId, varId, vertCoord.sigma[VertCoord::GridType::HALF].memptr());
            CHECK_NC_GET_VAR(ret, filePath, "ilev");
        }
        domain.setAxis(2, "lev", "classic sigma", "1", 0, RIGID, 1, RIGID);
        mesh().setGridCoordComps(2, len, vertCoord.sigma[VertCoord::GridType::FULL], vertCoord.sigma[VertCoord::GridType::HALF]);
        ret = nc_inq_varid(fileId, "pmtop", &varId);
        CHECK_NC_INQ_VARID(ret, filePath, "pmtop");
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileId, varId, "units", units);
        CHECK_NC_GET_ATT(ret, filePath, "pmtop", "units");
        assert(strcmp(units, "Pa") == 0);
        ret = nc_get_var_double(fileId, varId, &vertCoord.pt);
        CHECK_NC_GET_VAR(ret, filePath, "pmtop");
    } else if (domain.vertCoord().type() == HYBRID_PRESSURE_SIGMA) {
        REPORT_ERROR("Under construction!");
    } else if (domain.vertCoord().type() == PRESSURE) {
        vec full(len);
        ret = nc_get_var(fileId, fullVarIDs[2], full.memptr());
        CHECK_NC_GET_VAR(ret, filePath, domain.axisName(2));
        memset(units, 0, sizeof(units));
        ret = nc_get_att_text(fileId, fullVarIDs[2], "units", units);
        CHECK_NC_GET_ATT(ret, filePath, domain.axisName(2), "units");
        if (strcmp(units, "hPa") == 0) {
            full *= 100;
        }
        domain.setAxis(2, "lev", "pressure levels", "Pa",
                       full[0], RIGID, full[len-1], RIGID);
        mesh().setGridCoordComps(2, len, full);
    } else {
        REPORT_ERROR("Under construction!");
    }
} // inputVerticalMesh

void RLLDataFile::
outputMesh() {
    const auto &domain = mesh().domain();
    int ret;
    // write units
    ret = nc_redef(fileId);
    for (uword m = 0; m < domain.numDim(); ++m) {
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
        ret = nc_put_att(fileId, fullVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, filePath, domain.axisName(m), "units");
        ret = nc_put_att(fileId, halfVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, filePath, domain.axisName(m)+"_bnds", "units");
    }
    ret = nc_enddef(fileId);
    // write spatial grids
    for (uword m = 0; m < domain.numDim(); ++m) {
        if (m == 0 || m == 1) {
            vec x = mesh().gridCoordComps(m, GridType::FULL)/RAD;
            ret = nc_put_var(fileId, fullVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileId, fullVarIDs[m],
                             mesh().gridCoordComps(m, GridType::FULL).memptr());
        }
        CHECK_NC_GET_VAR(ret, filePath, domain.axisName(m));
        if (m == 0 || m == 1) {
            vec x = mesh().gridCoordComps(m, GridType::HALF)/RAD;
            ret = nc_put_var(fileId, halfVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileId, halfVarIDs[m],
                             mesh().gridCoordComps(m, GridType::HALF).memptr());
        }
        CHECK_NC_GET_VAR(ret, filePath, domain.axisName(m)+"_bnds");
    }
} // outputMesh

void RLLDataFile::
outputDomain() {
    const auto &domain = mesh().domain();
    int ret;
    char str[100];
    double value;
    ret = nc_redef(fileId);
    sprintf(str, "Sphere %llud", domain.numDim());
    ret = nc_put_att(fileId, NC_GLOBAL, "domain_type", NC_CHAR, 9, str);
    CHECK_NC_PUT_ATT(ret, filePath, "global", "domain_type");
    value = domain.radius();
    ret = nc_put_att(fileId, NC_GLOBAL, "sphere_radius", NC_DOUBLE, 1, &value);
    CHECK_NC_PUT_ATT(ret, filePath, "global", "sphere_radius");
    ret = nc_enddef(fileId);
} // outputDomain

} // geomtk
