#ifndef __Geomtk_IOManager_test__
#define __Geomtk_IOManager_test__

#include "geomtk.h"

using namespace geomtk;

class IOManagerTest : public ::testing::Test {
protected:
    TimeManager timeManager;
    IOManager<RLLDataFile> ioManager;
    SphereDomain *domain;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;
    
    void SetUp() {
        Time startTime(0), endTime(86400);
        
        timeManager.init(startTime, endTime, 360);
        ioManager.init(timeManager);
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        mesh->init(10, 10);
    }
    
    void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(IOManagerTest, OutputField) {
    RLLField<double, 2> f1, f2, f3;
    f1.create("f1", "test units", "a field on CENTER location", *mesh, RLLStagger::Location::CENTER);
    f2.create("f2", "test units", "a field on X_FACE location", *mesh, RLLStagger::Location::X_FACE);
    f3.create("f3", "test units", "a field on Y_FACE location", *mesh, RLLStagger::Location::Y_FACE);

    for (int i = 0; i < mesh->getTotalNumGrid(f1.getStaggerLocation()); ++i) {
        f1(timeIdx, i) = 1;
    }
    for (int i = 0; i < mesh->getTotalNumGrid(f2.getStaggerLocation()); ++i) {
        f2(timeIdx, i) = 2;
    }
    for (int i = 0; i < mesh->getTotalNumGrid(f3.getStaggerLocation()); ++i) {
        f3(timeIdx, i) = 3;
    }

    int fileIdx = ioManager.registerOutputFile(*mesh, "test-output");
    ioManager.file(fileIdx).registerOutputField<double, 2, RLLSpaceDimensions::FULL_DIMENSION>({&f1, &f2, &f3});
    ioManager.create(fileIdx);
    ioManager.output<double, 2>(fileIdx, timeIdx, {&f1, &f2});
    ioManager.output<double, 2>(fileIdx, timeIdx, {&f3});
    ioManager.close(fileIdx);

    int fileID, unlimDimID, timeDimID;
    int lonDimID, lonBndsDimID, latDimID, latBndsDimID;
    int varID, numDim, dimIDs[3], numAtt, ret;
    nc_type xtype;
    double *x;
    size_t attLen;
    char *att;

    ret = nc_open("test-output.000.nc", NC_NOWRITE, &fileID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_unlimdim(fileID, &unlimDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileID, "time", &timeDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(timeDimID, unlimDimID);

    ret = nc_inq_dimid(fileID, "lon", &lonDimID);
    ASSERT_EQ(NC_NOERR, ret);

    x = new double[mesh->getNumGrid(0, RLLStagger::GridType::FULL)];
    ret = nc_get_var(fileID, lonDimID, x);
    for (int i = 0; i < mesh->getNumGrid(0, RLLStagger::GridType::FULL); ++i) {
        ASSERT_EQ(mesh->getGridCoordComp(0, RLLStagger::GridType::FULL, i)/RAD, x[i]);
    }
    delete [] x;

    ret = nc_inq_dimid(fileID, "lon_bnds", &lonBndsDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileID, "lat", &latDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileID, "lat_bnds", &latBndsDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varid(fileID, "f1", &varID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileID, varID, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileID, varID, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimID);
    ASSERT_EQ(dimIDs[1], latDimID);
    ASSERT_EQ(dimIDs[2], lonDimID);

    x = new double[mesh->getTotalNumGrid(f1.getStaggerLocation())];
    ret = nc_get_var(fileID, varID, x);
    for (int i = 0; i < mesh->getTotalNumGrid(f1.getStaggerLocation()); ++i) {
        ASSERT_EQ(x[i], f1(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileID, varID, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileID, varID, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f1.getLongName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileID, varID, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f1.getUnits().c_str(), att);
    delete [] att;

    ret = nc_inq_varid(fileID, "f2", &varID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileID, varID, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileID, varID, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimID);
    ASSERT_EQ(dimIDs[1], latDimID);
    ASSERT_EQ(dimIDs[2], lonBndsDimID);

    x = new double[mesh->getTotalNumGrid(f2.getStaggerLocation())];
    ret = nc_get_var(fileID, varID, x);
    for (int i = 0; i < mesh->getTotalNumGrid(f2.getStaggerLocation()); ++i) {
        ASSERT_EQ(x[i], f2(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileID, varID, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileID, varID, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f2.getLongName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileID, varID, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f2.getUnits().c_str(), att);
    delete [] att;

    ret = nc_inq_varid(fileID, "f3", &varID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileID, varID, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileID, varID, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimID);
    ASSERT_EQ(dimIDs[1], latBndsDimID);
    ASSERT_EQ(dimIDs[2], lonDimID);

    x = new double[mesh->getTotalNumGrid(f3.getStaggerLocation())];
    ret = nc_get_var(fileID, varID, x);
    for (int i = 0; i < mesh->getTotalNumGrid(f3.getStaggerLocation()); ++i) {
        ASSERT_EQ(x[i], f3(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileID, varID, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileID, varID, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f3.getLongName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileID, varID, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileID, varID, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f3.getUnits().c_str(), att);
    delete [] att;

    ret = std::remove("test-output.000.nc");
    ASSERT_EQ(0, ret);
}

#endif // __Geomtk_IOManager_test__
