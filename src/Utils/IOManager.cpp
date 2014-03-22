#include "IOManager.h"

namespace geomtk {

StructuredDataFile::StructuredDataFile(const StructuredMesh &mesh) : DataFile() {
    this->mesh = &mesh;
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        fullDimIDs.resize(mesh.getNumGrid(m, GridType::FULL));
        fullVarIDs.resize(mesh.getNumGrid(m, GridType::FULL));
        halfDimIDs.resize(mesh.getNumGrid(m, GridType::HALF));
        halfVarIDs.resize(mesh.getNumGrid(m, GridType::HALF));
    }
}
    
void StructuredDataFile::removeOutputField(int numField, ...) {
    va_list fields;
    va_start(fields, numField);
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        bool found = false;
        for (int j = 0; j < fieldInfos.size(); ++j) {
            if (fieldInfos[j].field == field) {
                fieldInfos.erase(fieldInfos.begin()+j);
                found = true;
                break;
            }
        }
        if (!found) {
            REPORT_ERROR("Field is not in the output file!");
        }
    }
    va_end(fields);
}

void StructuredDataFile::create(const TimeManager &timeManager) {
    const Domain &domain = mesh->getDomain();
    string name, longName, units;
    int ret;
    // define spatial dimensions and their coordinate variables
    for (int m = 0; m < domain.getNumDim(); ++m) {
        name = domain.getAxisName(m);
        longName = domain.getAxisLongName(m);
        // full grids
        ret = nc_def_dim(fileID, name.c_str(),
                         mesh->getNumGrid(m, GridType::FULL),
                         &fullDimIDs[m]);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to define dimension \"" << name <<
                         "\" with error message \"" << nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
        ret = nc_def_var(fileID, name.c_str(), NC_DOUBLE, 1,
                         &fullDimIDs[m], &fullVarIDs[m]);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to define variable \"" << name <<
                         "\" with error message \"" << nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
        ret = nc_put_att(fileID, fullVarIDs[m],
                         "long_name", NC_CHAR,
                         longName.length(), longName.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"long_name\" attribute to " <<
                         "variable \"" << name << "\" with error " <<
                         "message \"" << nc_strerror(ret) << "\" " <<
                         "in file \"" << fileName << "\"!");
        }
        // half grids
        name += "_bnds";
        ret = nc_def_dim(fileID, name.c_str(),
                         mesh->getNumGrid(m, GridType::HALF),
                         &halfDimIDs[m]);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to define dimension \"" << name <<
                         "\" with error message \"" << nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
        ret = nc_def_var(fileID, name.c_str(), NC_DOUBLE, 1,
                         &halfDimIDs[m], &halfVarIDs[m]);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to define variable \"" << name <<
                         "\" with error message \"" << nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
    }
    // define temporal dimension
    ret = nc_def_dim(fileID, "time", NC_UNLIMITED, &timeDimID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to define dimension \"time\" with error " <<
                     "message \"" << nc_strerror(ret) << "\"!");
    }
    ret = nc_def_var(fileID, "time", NC_DOUBLE, 1,
                     &timeDimID, &timeVarID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to define variable \"time\" with erro " <<
                     "message \"" << nc_strerror(ret) << "\"!");
    }
    units = "days since "+timeManager.getStartTime().s();
    ret = nc_put_att(fileID, timeVarID, "units", NC_CHAR,
                     units.length(), units.c_str());
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to put \"units\" attribute to variable \"time\"!");
    }
    // define fields
    for (int i = 0; i < fieldInfos.size(); ++i) {
        name = fieldInfos[i].field->getName();
        longName = fieldInfos[i].field->getLongName();
        units = fieldInfos[i].field->getUnits();
        vector<int> dimIDs;
        int l = 1;
        switch (fieldInfos[i].field->getStaggerLocation()) {
            case Location::CENTER:
                switch (fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.getNumDim()+1);
                        dimIDs[0] = timeDimID;
                        for (int m = domain.getNumDim()-1; m >= 0; --m) {
                            dimIDs[l++] = fullDimIDs[m];
                        }
                        break;
                    case SpaceDimensions::XY_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::XZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::YZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    default:
                        REPORT_ERROR("Unknown spatial dimension!");
                }
                break;
            case Location::X_FACE:
                switch (fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.getNumDim()+1);
                        dimIDs[0] = timeDimID;
                        for (int m = domain.getNumDim()-1; m >= 0; --m) {
                            if (m == 0) {
                                dimIDs[l++] = halfDimIDs[m];
                            } else {
                                dimIDs[l++] = fullDimIDs[m];
                            }
                        }
                        break;
                    case SpaceDimensions::XY_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::XZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::YZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    default:
                        REPORT_ERROR("Unknown spatial dimension!");
                }
                break;
            case Location::Y_FACE:
                switch (fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.getNumDim()+1);
                        dimIDs[0] = timeDimID;
                        for (int m = domain.getNumDim()-1; m >= 0; --m) {
                            if (m == 1) {
                                dimIDs[l++] = halfDimIDs[m];
                            } else {
                                dimIDs[l++] = fullDimIDs[m];
                            }
                        }
                        break;
                    case SpaceDimensions::XY_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::XZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::YZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    default:
                        REPORT_ERROR("Unknown spatial dimension!");
                }
                break;
            case Location::Z_FACE:
                switch (fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.getNumDim()+1);
                        dimIDs[0] = timeDimID;
                        for (int m = domain.getNumDim()-1; m >= 0; --m) {
                            if (m == 2) {
                                dimIDs[l++] = halfDimIDs[m];
                            } else {
                                dimIDs[l++] = fullDimIDs[m];
                            }
                        }
                        break;
                    case SpaceDimensions::XY_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::XZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::YZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    default:
                        REPORT_ERROR("Unknown spatial dimension!");
                }
                break;
            case Location::XY_VERTEX:
                switch (fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.getNumDim()+1);
                        dimIDs[0] = timeDimID;
                        for (int m = domain.getNumDim()-1; m >= 0; --m) {
                            if (m == 0 || m == 1) {
                                dimIDs[l++] = halfDimIDs[m];
                            } else {
                                dimIDs[l++] = fullDimIDs[m];
                            }
                        }
                        break;
                    case SpaceDimensions::XY_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::XZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    case SpaceDimensions::YZ_DIMENSION:
                        REPORT_ERROR("Under construction!");
                        break;
                    default:
                        REPORT_ERROR("Unknown spatial dimension!");
                }
                break;
            default:
                REPORT_ERROR("Unknown stagger location!");
        }
        ret = nc_def_var(fileID, name.c_str(), fieldInfos[i].xtype,
                         dimIDs.size(), &dimIDs[0], &fieldInfos[i].varID);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to define variable \"" << name <<
                         "\" with erro message \"" <<
                         nc_strerror(ret) << "\"!");
        }
        ret = nc_put_att(fileID, fieldInfos[i].varID, "long_name", NC_CHAR,
                         longName.length(), longName.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"long_name\" attribute to " <<
                         "variable \"" << name << "\" with error " <<
                         "message \"" << nc_strerror(ret) << "\" " <<
                         "in file \"" << fileName << "\"!");
        }
        ret = nc_put_att(fileID, fieldInfos[i].varID, "units", NC_CHAR,
                         units.length(), units.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"units\" attribute to " <<
                         "variable \"" << name << "\" with error " <<
                         "message \"" << nc_strerror(ret) << "\" " <<
                         "in file \"" << fileName << "\"!");
        }
    }
    // turn off define mode
    ret = nc_enddef(fileID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to end definition for file \"" <<
                     fileName <<  "\" with error message \"" <<
                     nc_strerror(ret) << "\"!");
    }
}

void StructuredDataFile::outputGrids() {
    const Domain &domain = mesh->getDomain();
    int ret;
    // write units
    ret = nc_redef(fileID);
    for (int m = 0; m < domain.getNumDim(); ++m) {
        string units = domain.getAxisUnits(m);
        ret = nc_put_att(fileID, fullVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"units\" attribute to " <<
                         "variable \"" << domain.getAxisName(m) << "\" " <<
                         "with error message \""<< nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
        ret = nc_put_att(fileID, halfVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"units\" attribute to " <<
                         "variable \"" << domain.getAxisName(m) <<
                         "_bnds\" with error message \""<<
                         nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }

    }
    ret = nc_enddef(fileID);
    // write spatial grids
    for (int m = 0; m < domain.getNumDim(); ++m) {
        ret = nc_put_var(fileID, fullVarIDs[m],
                         mesh->getGridCoords(m, GridType::FULL).memptr());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put variable \"" <<
                         domain.getAxisName(m) << "\" with error " <<
                         "message \"" << nc_strerror(ret) << "\"");
        }
        ret = nc_put_var(fileID, halfVarIDs[m],
                         mesh->getGridCoords(m, GridType::HALF).memptr());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put variable \"" <<
                         domain.getAxisName(m)+"_bnds" << "\" with " <<
                         "error message \"" << nc_strerror(ret) << "\"!");
        }
    }
}

void RLLDataFile::outputGrids() {
    const Domain &domain = mesh->getDomain();
    int ret;
    // write units
    ret = nc_redef(fileID);
    for (int m = 0; m < domain.getNumDim(); ++m) {
        string units = domain.getAxisUnits(m);
        if (m == 0) {
#ifdef DEBUG
            assert(units == "radian_east");
#endif
            units = "degree_east";
        } else if (m == 1) {
#ifdef DEBUG
            assert(units == "radian_north");
#endif
            units = "degree_north";
        }
        ret = nc_put_att(fileID, fullVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"units\" attribute to " <<
                         "variable \"" << domain.getAxisName(m) << "\" " <<
                         "with error message \""<< nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
        ret = nc_put_att(fileID, halfVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put \"units\" attribute to " <<
                         "variable \"" << domain.getAxisName(m) <<
                         "_bnds\" with error message \""<<
                         nc_strerror(ret) <<
                         "\" in file \"" << fileName << "\"!");
        }
    }
    ret = nc_enddef(fileID);
    // write spatial grids
    for (int m = 0; m < domain.getNumDim(); ++m) {
        if (m == 0 || m == 1) {
            vec x = mesh->getGridCoords(m, GridType::FULL)/RAD;
            ret = nc_put_var(fileID, fullVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileID, fullVarIDs[m],
                             mesh->getGridCoords(m, GridType::FULL).memptr());
        }
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put variable \"" <<
                         domain.getAxisName(m) << "\" with error " <<
                         "message \"" << nc_strerror(ret) << "\"");
        }
        if (m == 0 || m == 1) {
            vec x = mesh->getGridCoords(m, GridType::HALF)/RAD;
            ret = nc_put_var(fileID, halfVarIDs[m], x.memptr());
        } else {
            ret = nc_put_var(fileID, halfVarIDs[m],
                             mesh->getGridCoords(m, GridType::HALF).memptr());
        }
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put variable \"" <<
                         domain.getAxisName(m)+"_bnds" << "\" with " <<
                         "error message \"" << nc_strerror(ret) << "\"!");
        }
    }
}

}
