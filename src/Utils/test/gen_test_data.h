#ifndef __GEOMTK_GenTestData__
#define __GEOMTK_GenTestData__

void gen_2d_tv_data(const string &filePath)
{
    int fileId, lonDimID, lonVarID, latDimID, latVarID, timeDimId, timeVarId;
    int varId, dimIDs[3];
    int ret;

    int numLon = 10, numLat = 10;
    double lon[numLon], lat[numLat];
    float x[numLat][numLon];

    ret = nc_create(filePath.c_str(), NC_CLOBBER, &fileId);

    ret = nc_def_dim(fileId, "lon", numLon, &lonDimID);

    ret = nc_def_var(fileId, "lon", NC_DOUBLE, 1, &lonDimID, &lonVarID);

    ret = nc_put_att(fileId, lonVarID, "long_name", NC_CHAR, 9, "longitude");

    ret = nc_put_att(fileId, lonVarID, "units", NC_CHAR, 13, "degree_east");

    ret = nc_def_dim(fileId, "lat", numLat, &latDimID);

    ret = nc_def_var(fileId, "lat", NC_DOUBLE, 1, &latDimID, &latVarID);

    ret = nc_put_att(fileId, latVarID, "long_name", NC_CHAR, 8, "latitude");

    ret = nc_put_att(fileId, latVarID, "units", NC_CHAR, 12, "degree_north");

    ret = nc_def_dim(fileId, "time", NC_UNLIMITED, &timeDimId);

    ret = nc_def_var(fileId, "time", NC_INT, 1, &timeDimId, &timeVarId);

    ret = nc_put_att(fileId, timeVarId, "long_name", NC_CHAR, 4, "time");

    ret = nc_put_att(fileId, timeVarId, "units", NC_CHAR, 33, "minutes since 0001-01-01 00:00:00");

    dimIDs[0] = timeDimId;
    dimIDs[1] = latDimID;
    dimIDs[2] = lonDimID;

    ret = nc_def_var(fileId, "x", NC_FLOAT, 3, dimIDs, &varId);

    ret = nc_put_att(fileId, varId, "long_name", NC_CHAR, 13, "test variable");

    ret = nc_put_att(fileId, varId, "units", NC_CHAR, 1, "1");

    ret = nc_enddef(fileId);

    double dlon = 360/numLon;
    for (int i = 0; i < numLon; ++i) {
        lon[i] = i*dlon;
    }
    double dlat = 180/(numLat-1);
    for (int j = 0; j < numLat; ++j) {
        lat[j] = -90+j*dlat;
    }
    for (int j = 0; j < numLat; ++j) {
        for (int i = 0; i < numLon; ++i) {
            x[i][j] = j+i*numLat;
        }
    }

    ret = nc_put_var(fileId, lonVarID, lon);

    ret = nc_put_var(fileId, latVarID, lat);

    int time = 0;
    size_t index[1] = {0};

    ret = nc_put_var1(fileId, timeVarId, index, &time);

    ret = nc_put_var(fileId, varId, &x[0][0]);

    ret = nc_close(fileId);
}

#endif // __GEOMTK_GenTestData__
