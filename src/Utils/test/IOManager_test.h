#ifndef __Geomtk_IOManager_test__
#define __Geomtk_IOManager_test__

#include "geomtk.h"
#include "gen_test_data.h"

using namespace geomtk;

class IOManagerTest : public ::testing::Test {
protected:
    TimeManager timeManager;
    IOManager<RLLDataFile> ioManager;
    SphereDomain *domain;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;
    StampString filePattern;

    void SetUp() {
        Time startTime(0), endTime(1*TimeUnit::DAYS);
        filePattern.init("test-output.%5s.nc");
        timeManager.init(startTime, endTime, 1*TimeUnit::MINUTES);
        ioManager.init(timeManager);
    }
    
    void TearDown() {
        delete mesh;
        delete domain;
    }
};

TEST_F(IOManagerTest, Input2DMesh) {
    domain = new SphereDomain(2);
    mesh = new RLLMesh(*domain);

    gen_2d_tv_data("test.nc");
    mesh->init("test.nc");
    double dlon = PI2/10;
    for (int i = 0; i < 10; ++i) {
        ASSERT_LE(fabs(mesh->getGridCoordComp(0, RLLStagger::GridType::FULL, i)-i*dlon), 1.0e-15);
        ASSERT_LE(fabs(mesh->getGridCoordComp(0, RLLStagger::GridType::HALF, i)-(i+0.5)*dlon), 1.0e-15);
    }
    double dlat = M_PI/9;
    for (int j = 0; j < 10; ++j) {
        ASSERT_LE(fabs(mesh->getGridCoordComp(1, RLLStagger::GridType::FULL, j)+M_PI_2-j*dlat), 1.0e-15);
        if (j == 9) continue;
        ASSERT_LE(fabs(mesh->getGridCoordComp(1, RLLStagger::GridType::HALF, j)+M_PI_2-(j+0.5)*dlat), 1.0e-15);
    }
    ASSERT_LE(fabs(mesh->getGridCoordComp(1, RLLStagger::GridType::FULL, 9)-M_PI_2), 1.0e-15);
    SystemTools::removeFile("test.nc");
}

TEST_F(IOManagerTest, OutputFrequency) {
    domain = new SphereDomain(2);
    mesh = new RLLMesh(*domain);
    mesh->init(10, 10);

    int fileIdx = ioManager.registerOutputFile(*mesh, filePattern, MINUTE, 5);
    RLLDataFile &dataFile = ioManager.files[fileIdx];
    ASSERT_FALSE(dataFile.isActive);
    while (!timeManager.isFinished()) {
        ioManager.isFileActive(fileIdx);
        double seconds = timeManager.getSeconds();
        if (fmod(seconds, 300) == 0) {
            ASSERT_TRUE(dataFile.isActive);
        }
        timeManager.advance(true);
    }
}

TEST_F(IOManagerTest, OutputField) {
    domain = new SphereDomain(2);
    mesh = new RLLMesh(*domain);
    mesh->init(10, 10);

    RLLField<double, 2> f1, f2, f3;
    f1.create("f1", "test units", "a field on CENTER location", *mesh, RLLStagger::Location::CENTER, 2, false);
    f2.create("f2", "test units", "a field on X_FACE location", *mesh, RLLStagger::Location::X_FACE, 2, false);
    f3.create("f3", "test units", "a field on Y_FACE location", *mesh, RLLStagger::Location::Y_FACE, 2, false);

    for (int i = 0; i < mesh->getTotalNumGrid(f1.getStaggerLocation(), f1.getNumDim()); ++i) {
        f1(timeIdx, i) = 1;
    }
    for (int i = 0; i < mesh->getTotalNumGrid(f2.getStaggerLocation(), f2.getNumDim()); ++i) {
        f2(timeIdx, i) = 2;
    }
    for (int i = 0; i < mesh->getTotalNumGrid(f3.getStaggerLocation(), f3.getNumDim()); ++i) {
        f3(timeIdx, i) = 3;
    }

    int fileIdx = ioManager.registerOutputFile(*mesh, filePattern, STEP, 1);
    ioManager.file(fileIdx).registerField("double", RLLSpaceDimensions::FULL_DIMENSION, {&f1, &f2, &f3});
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

    ret = nc_open("test-output.00000.nc", NC_NOWRITE, &fileID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_unlimdim(fileID, &unlimDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileID, "time", &timeDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(timeDimID, unlimDimID);

    ret = nc_inq_dimid(fileID, "lon", &lonDimID);
    ASSERT_EQ(NC_NOERR, ret);

    x = new double[mesh->getNumGrid(0, RLLStagger::GridType::FULL)];
    ret = nc_get_var_double(fileID, lonDimID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (int i = 0; i < mesh->getNumGrid(0, RLLStagger::GridType::FULL); ++i) {
        ASSERT_GE(1.0e-15, fabs(mesh->getGridCoordComp(0, RLLStagger::GridType::FULL, i)/RAD-x[i]));
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

    x = new double[mesh->getTotalNumGrid(f1.getStaggerLocation(), f1.getNumDim())];
    ret = nc_get_var_double(fileID, varID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (int i = 0; i < mesh->getTotalNumGrid(f1.getStaggerLocation(), f1.getNumDim()); ++i) {
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

    x = new double[mesh->getTotalNumGrid(f2.getStaggerLocation(), f2.getNumDim())];
    ret = nc_get_var_double(fileID, varID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (int i = 0; i < mesh->getTotalNumGrid(f2.getStaggerLocation(), f2.getNumDim()); ++i) {
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

    x = new double[mesh->getTotalNumGrid(f3.getStaggerLocation(), f3.getNumDim())];
    ret = nc_get_var_double(fileID, varID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (int i = 0; i < mesh->getTotalNumGrid(f3.getStaggerLocation(), f3.getNumDim()); ++i) {
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

    ret = nc_close(fileID);

    SystemTools::removeFile("test-output.00000.nc");
}

#endif // __Geomtk_IOManager_test__
