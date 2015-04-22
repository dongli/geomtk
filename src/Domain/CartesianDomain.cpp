#include "CartesianDomain.h"

namespace geomtk {

void CartesianDomain::
init(const string &filePath) {
    int ncId, varId, ret; char str1[100], str2[100];
    sregex reDomainType = sregex::compile("(\\w+) (\\d)d");
    sregex reSpan = sregex::compile("(\\d*(\\.\\d*)?)-(\\d*(\\.\\d*)?)");
    sregex reBndCond = sregex::compile("(\\w+)-(\\w+)");
    smatch what;
    ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
    CHECK_NC_OPEN(ret, filePath);
    memset(&str1[0], 0, sizeof(str1)); ret = nc_get_att_text(ncId, NC_GLOBAL, "domain_type", str1);
    CHECK_NC_GET_ATT(ret, filePath, "global", "domain_type");
    if (regex_search(string(str1), what, reDomainType)) {
        string domainType = what[1];
        assert(domainType == "Cartesian");
        string numDim = what[2];
        assert(_numDim == atoi(numDim.c_str()));
        char axisName[10];
        for (int m = 0; m < _numDim; ++m) {
            if (m == 0) {
                sprintf(axisName, "%s", "x");
            } else if (m == 1) {
                sprintf(axisName, "%s", "y");
            } else if (m == 2) {
                sprintf(axisName, "%s", "z");
            }
            sprintf(str2, "domain_%s_span", axisName);
            memset(&str1[0], 0, sizeof(str1)); ret = nc_get_att_text(ncId, NC_GLOBAL, str2, str1);
            CHECK_NC_GET_ATT(ret, filePath, "global", str2);
            if (regex_search(string(str1), what, reSpan)) {
                sprintf(str2, "domain_%s_boundary_condition", axisName);
                double start = atof(string(what[1]).c_str());
                double end = atof(string(what[3]).c_str());
                memset(&str1[0], 0, sizeof(str1)); ret = nc_get_att_text(ncId, NC_GLOBAL, str2, str1);
                CHECK_NC_GET_ATT(ret, filePath, "global", str2);
                if (regex_search(string(str1), what, reBndCond)) {
                    ret = nc_inq_varid(ncId, axisName, &varId);
                    CHECK_NC_INQ_VARID(ret, filePath, axisName);
                    memset(&str1[0], 0, sizeof(str1)); ret = nc_get_att_text(ncId, varId, "long_name", str1);
                    CHECK_NC_GET_ATT(ret, filePath, axisName, str2);
                    string longName(str1);
                    memset(&str1[0], 0, sizeof(str1)); ret = nc_get_att_text(ncId, varId, "units", str1);
                    CHECK_NC_GET_ATT(ret, filePath, axisName, str2);
                    string units(str1);
                    setAxis(m, axisName, longName, units,
                            start, bndTypeFromString(what[1]),
                            end, bndTypeFromString(what[2]));
                }
            }
        }
    }
    ret = nc_close(ncId);
    CHECK_NC_CLOSE(ret, filePath);
}

vec CartesianDomain::
diffCoord(const SpaceCoord &x, const SpaceCoord &y) const {
    vec res = x()-y();
    double tmp1, tmp2;
    for (int m = 0; m < numDim(); ++m) {
        if (axisStartBndType(m) == PERIODIC) {
            tmp1 = fabs(res[m]);
            tmp2 = axisSpan(m)-tmp1;
            if (tmp1 > tmp2) {
                res[m] = res[m] > 0 ? -tmp2 : tmp2;
            }
        }
    }
    return res;
}

double CartesianDomain::
calcDistance(const SpaceCoord &x, const SpaceCoord &y) const {
    return norm(diffCoord(x, y));
}

}