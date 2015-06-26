#include "CartesianDataFile.h"

namespace geomtk {

void CartesianDataFile::
inputHorizontalMesh() {

} // inputHorizontalMesh

void CartesianDataFile::
inputVerticalMesh() {

} // inputVerticalMesh

void CartesianDataFile::
outputDomain() {
    const auto &domain = mesh().domain();
    int ret; char str1[100], str2[100];
    ret = nc_redef(fileId);
    sprintf(str1, "Cartesian %llud", domain.numDim());
    ret = nc_put_att(fileId, NC_GLOBAL, "domain_type", NC_CHAR, 12, str1);
    CHECK_NC_PUT_ATT(ret, filePath, "global", "domain_type");
    for (uword m = 0; m < domain.numDim(); ++m) {
        if (m == 0) {
            sprintf(str2, "x");
        } else if (m == 1) {
            sprintf(str2, "y");
        } else if (m == 2) {
            sprintf(str2, "z");
        }
        sprintf(str1, "domain_%s_boundary_condition", str2);
        sprintf(str2, "%s-%s",
                bndTypeToString(domain.axisStartBndType(m)).c_str(),
                bndTypeToString(domain.axisEndBndType(m)).c_str());
        ret = nc_put_att(fileId, NC_GLOBAL, str1, NC_CHAR, strlen(str2), str2);
        CHECK_NC_PUT_ATT(ret, filePath, "global", str1);
        sprintf(str1, "domain_%s_span", str2);
        sprintf(str2, "%f-%f", domain.axisStart(m), domain.axisEnd(m));
        ret = nc_put_att(fileId, NC_GLOBAL, str1, NC_CHAR, strlen(str2), str2);
        CHECK_NC_PUT_ATT(ret, filePath, "global", str1);
    }
    ret = nc_enddef(fileId);
} // outputDomain

} // geomtk
