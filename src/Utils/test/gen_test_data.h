#ifndef __Geomtk_GenTestData__
#define __Geomtk_GenTestData__

void gen_2d_tv_data(const string &filePath)
{
    int fileID, lonDimID, lonVarID, latDimID, latVarID, timeDimID, timeVarID;
    int varID, dimIDs[3];
    int ret;

    int numLon = 10, numLat = 10;
    double lon[numLon], lat[numLat];
    float x[numLat][numLon];

    ret = nc_create(filePath.c_str(), NC_CLOBBER, &fileID);

    ret = nc_def_dim(fileID, "lon", numLon, &lonDimID);

    ret = nc_def_var(fileID, "lon", NC_DOUBLE, 1, &lonDimID, &lonVarID);

    ret = nc_put_att(fileID, lonVarID, "long_name", NC_CHAR, 9, "longitude");

    ret = nc_put_att(fileID, lonVarID, "units", NC_CHAR, 13, "degree_east");

    ret = nc_def_dim(fileID, "lat", numLat, &latDimID);

    ret = nc_def_var(fileID, "lat", NC_DOUBLE, 1, &latDimID, &latVarID);

    ret = nc_put_att(fileID, latVarID, "long_name", NC_CHAR, 8, "latitude");

    ret = nc_put_att(fileID, latVarID, "units", NC_CHAR, 12, "degree_north");

    ret = nc_def_dim(fileID, "time", NC_UNLIMITED, &timeDimID);

    ret = nc_def_var(fileID, "time", NC_INT, 1, &timeDimID, &timeVarID);

    ret = nc_put_att(fileID, timeVarID, "long_name", NC_CHAR, 4, "time");

    ret = nc_put_att(fileID, timeVarID, "units", NC_CHAR, 33, "minutes since 0001-01-01 00:00:00");

    dimIDs[0] = timeDimID;
    dimIDs[1] = latDimID;
    dimIDs[2] = lonDimID;

    ret = nc_def_var(fileID, "x", NC_FLOAT, 3, dimIDs, &varID);

    ret = nc_put_att(fileID, varID, "long_name", NC_CHAR, 13, "test variable");

    ret = nc_put_att(fileID, varID, "units", NC_CHAR, 1, "1");

    ret = nc_enddef(fileID);

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

    ret = nc_put_var(fileID, lonVarID, lon);

    ret = nc_put_var(fileID, latVarID, lat);

    int time = 0;
    size_t index[1] = {0};

    ret = nc_put_var1(fileID, timeVarID, index, &time);

    ret = nc_put_var(fileID, varID, &x[0][0]);

    ret = nc_close(fileID);
}

#endif // __Geomtk_GenTestData__
