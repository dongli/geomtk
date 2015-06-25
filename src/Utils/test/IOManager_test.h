#ifndef __GEOMTK_IOManager_test__
#define __GEOMTK_IOManager_test__

#include "geomtk.h"
#include "gen_test_data.h"

using namespace geomtk;

class IOManagerTest : public ::testing::Test {
protected:
    const int FULL = RLLStagger::GridType::FULL;
    const int HALF = RLLStagger::GridType::HALF;
    const int CENTER = RLLStagger::Location::CENTER;
    const int X_FACE = RLLStagger::Location::X_FACE;
    const int Y_FACE = RLLStagger::Location::Y_FACE;

    typedef IOManager<RLLDataFile> IOManager;

    TimeManager timeManager;
    IOManager ioManager;
    SphereDomain *domain;
    RLLMesh *mesh;
    TimeLevelIndex<2> timeIdx;
    StampString filePattern;

    void SetUp() {
        domain = new SphereDomain(2);
        mesh = new RLLMesh(*domain);
        ptime startTime(date(2000, 1, 1)), endTime(date(2000, 1, 2));
        filePattern.init("test-output.%5s.nc");
        timeManager.init(startTime, endTime, minutes(1));
        ioManager.init(timeManager);
        gen_2d_tv_data("test.nc");
    }
    
    void TearDown() {
        delete mesh;
        delete domain;
        SystemTools::removeFile("test.nc");
    }
};

TEST_F(IOManagerTest, GetTime) {
    ptime time = IOManager::getTime("test.nc");
    ASSERT_EQ(ptime(date(2015, 1, 1), time_duration(0, 23, 0)), time);
}

TEST_F(IOManagerTest, Input2DMesh) {
    mesh->init("test.nc");
    double dlon = PI2/mesh->numGrid(0, FULL);
    for (uword i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
        ASSERT_LE(fabs(mesh->gridCoordComp(0, FULL, i)-(i-1)*dlon), 1.0e-15);
    }
    for (uword i = mesh->is(HALF); i <= mesh->ie(HALF); ++i) {
        ASSERT_LE(fabs(mesh->gridCoordComp(0, HALF, i)-(i-0.5)*dlon), 1.0e-15);
    }
    double dlat = M_PI/(mesh->numGrid(1, FULL)-1);
    for (uword j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        ASSERT_LE(fabs(mesh->gridCoordComp(1, FULL, j)+M_PI_2-j*dlat), 1.0e-15);
    }
    for (uword j = mesh->js(HALF); j <= mesh->je(HALF); ++j) {
        ASSERT_LE(fabs(mesh->gridCoordComp(1, HALF, j)+M_PI_2-(j+0.5)*dlat), 1.0e-15);
    }
    ASSERT_LE(fabs(mesh->gridCoordComp(1, FULL, 9)-M_PI_2), 1.0e-15);
}

TEST_F(IOManagerTest, OutputFrequency) {
    mesh->init(10, 10);

    int fileIdx = ioManager.addOutputFile(*mesh, filePattern, minutes(5));
    RLLDataFile &dataFile = ioManager.files[fileIdx];
    ASSERT_FALSE(dataFile.isActive);
    while (!timeManager.isFinished()) {
        ioManager.isFileActive(fileIdx);
        double seconds = timeManager.seconds();
        if (fmod(seconds, 300) == 0) {
            ASSERT_TRUE(dataFile.isActive);
        }
        timeManager.advance(true);
    }
}

TEST_F(IOManagerTest, OutputField) {
    mesh->init(10, 10);

    RLLField<double, 2> f1, f2, f3;
    f1.create("f1", "test units", "a field on CENTER location", *mesh, CENTER, 2, false);
    f2.create("f2", "test units", "a field on X_FACE location", *mesh, X_FACE, 2, false);
    f3.create("f3", "test units", "a field on Y_FACE location", *mesh, Y_FACE, 2, false);

    for (uword i = 0; i < mesh->totalNumGrid(f1.staggerLocation(), f1.numDim()); ++i) {
        f1(timeIdx, i) = 1;
    }
    for (uword i = 0; i < mesh->totalNumGrid(f2.staggerLocation(), f2.numDim()); ++i) {
        f2(timeIdx, i) = 2;
    }
    for (uword i = 0; i < mesh->totalNumGrid(f3.staggerLocation(), f3.numDim()); ++i) {
        f3(timeIdx, i) = 3;
    }

    int fileIdx = ioManager.addOutputFile(*mesh, filePattern, seconds(-1));
    ioManager.file(fileIdx).addField("double", RLLSpaceDimensions::FULL_DIMENSION, {&f1, &f2, &f3});
    ioManager.create(fileIdx);
    ioManager.output<double, 2>(fileIdx, timeIdx, {&f1, &f2});
    ioManager.output<double, 2>(fileIdx, timeIdx, {&f3});
    ioManager.close(fileIdx);

    int fileId, unlimDimID, timeDimId;
    int lonDimID, lonBndsDimID, latDimID, latBndsDimID;
    int varId, numDim, dimIDs[3], numAtt, ret;
    nc_type xtype;
    double *x;
    size_t attLen;
    char *att;

    ret = nc_open("test-output.00000.nc", NC_NOWRITE, &fileId);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_unlimdim(fileId, &unlimDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileId, "time", &timeDimId);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(timeDimId, unlimDimID);

    ret = nc_inq_dimid(fileId, "lon", &lonDimID);
    ASSERT_EQ(NC_NOERR, ret);

    x = new double[mesh->numGrid(0, FULL)];
    ret = nc_get_var_double(fileId, lonDimID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (uword i = mesh->is(FULL); i <= mesh->ie(FULL); ++i) {
        // Note: 'i' starts from 1, so the index of 'x' is 'i-1'.
        ASSERT_GE(1.0e-15, fabs(mesh->gridCoordComp(0, FULL, i)/RAD-x[i-1]));
    }
    delete [] x;

    ret = nc_inq_dimid(fileId, "lon_bnds", &lonBndsDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_dimid(fileId, "lat", &latDimID);
    ASSERT_EQ(NC_NOERR, ret);

    x = new double[mesh->numGrid(1, FULL)];
    ret = nc_get_var_double(fileId, latDimID, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (uword j = mesh->js(FULL); j <= mesh->je(FULL); ++j) {
        ASSERT_GE(1.0e-15, fabs(mesh->gridCoordComp(1, FULL, j)/RAD-x[j]));
    }
    delete [] x;

    ret = nc_inq_dimid(fileId, "lat_bnds", &latBndsDimID);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varid(fileId, "f1", &varId);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileId, varId, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileId, varId, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimId);
    ASSERT_EQ(dimIDs[1], latDimID);
    ASSERT_EQ(dimIDs[2], lonDimID);

    x = new double[mesh->totalNumGrid(f1.staggerLocation(), f1.numDim())];
    ret = nc_get_var_double(fileId, varId, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (uword i = 0; i < mesh->totalNumGrid(f1.staggerLocation(), f1.numDim()); ++i) {
        ASSERT_EQ(x[i], f1(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileId, varId, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileId, varId, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f1.longName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileId, varId, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f1.units().c_str(), att);
    delete [] att;

    ret = nc_inq_varid(fileId, "f2", &varId);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileId, varId, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileId, varId, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimId);
    ASSERT_EQ(dimIDs[1], latDimID);
    ASSERT_EQ(dimIDs[2], lonBndsDimID);

    x = new double[mesh->totalNumGrid(f2.staggerLocation(), f2.numDim())];
    ret = nc_get_var_double(fileId, varId, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (uword i = 0; i < mesh->totalNumGrid(f2.staggerLocation(), f2.numDim()); ++i) {
        ASSERT_EQ(x[i], f2(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileId, varId, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileId, varId, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f2.longName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileId, varId, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f2.units().c_str(), att);
    delete [] att;

    ret = nc_inq_varid(fileId, "f3", &varId);
    ASSERT_EQ(NC_NOERR, ret);

    ret = nc_inq_varndims(fileId, varId, &numDim);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(3, numDim);

    ret = nc_inq_vardimid(fileId, varId, dimIDs);
    ASSERT_EQ(NC_NOERR, ret);

    ASSERT_EQ(dimIDs[0], timeDimId);
    ASSERT_EQ(dimIDs[1], latBndsDimID);
    ASSERT_EQ(dimIDs[2], lonDimID);

    x = new double[mesh->totalNumGrid(f3.staggerLocation(), f3.numDim())];
    ret = nc_get_var_double(fileId, varId, x);
    ASSERT_EQ(NC_NOERR, ret);
    for (uword i = 0; i < mesh->totalNumGrid(f3.staggerLocation(), f3.numDim()); ++i) {
        ASSERT_EQ(x[i], f3(timeIdx, i));
    }
    delete [] x;

    ret = nc_inq_varnatts(fileId, varId, &numAtt);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(2, numAtt);

    ret = nc_inq_att(fileId, varId, "long_name", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "long_name", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f3.longName().c_str(), att);
    delete [] att;

    ret = nc_inq_att(fileId, varId, "units", &xtype, &attLen);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_EQ(NC_CHAR, xtype);

    att = new char[attLen];
    ret = nc_get_att(fileId, varId, "units", att);
    ASSERT_EQ(NC_NOERR, ret);
    ASSERT_STREQ(f3.units().c_str(), att);
    delete [] att;

    ret = nc_close(fileId);

    SystemTools::removeFile("test-output.00000.nc");
}

#endif // __GEOMTK_IOManager_test__
