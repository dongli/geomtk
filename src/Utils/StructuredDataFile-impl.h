namespace geomtk {

template <class MeshType>
StructuredDataFile<MeshType>::StructuredDataFile(MeshType &mesh) : DataFile<MeshType>(mesh) {
    fullDimIDs.resize(mesh.domain().numDim());
    fullVarIDs.resize(mesh.domain().numDim());
    halfDimIDs.resize(mesh.domain().numDim());
    halfVarIDs.resize(mesh.domain().numDim());
    bnds2D = false;
}

template <class MeshType>
void StructuredDataFile<MeshType>::open(const TimeManager &timeManager) {
    const auto &domain = this->mesh().domain();
    int ret;
    // inquire full dimensions
    for (int m = 0; m < domain.numDim(); ++m) {
        ret = nc_inq_dimid(this->fileID, domain.axisName(m).c_str(), &fullDimIDs[m]);
        CHECK_NC_INQ_DIMID(ret, this->fileName, domain.axisName(m));
        if (this->mesh().isSet()) {
            // check if mesh is matched with file
            size_t len;
            ret = nc_inq_dimlen(this->fileID, fullDimIDs[m], &len);
            CHECK_NC_INQ_DIMLEN(ret, this->fileName, domain.axisName(m));
            if (len != this->mesh().numGrid(m, StructuredStagger::GridType::FULL)) {
                REPORT_ERROR("Dimension " << domain.axisName(m) <<
                             " length (" << len << " - " <<
                             this->mesh().numGrid(m, StructuredStagger::GridType::FULL)
                             << ") does not match!");
            }
        }
        ret = nc_inq_varid(this->fileID, domain.axisName(m).c_str(), &fullVarIDs[m]);
        CHECK_NC_INQ_VARID(ret, this->fileName, domain.axisName(m));
    }
    // check if the bounds are in 2D format
    ret = nc_inq_dimid(this->fileID, "bnds", &bndsDimID);
    if (ret == NC_NOERR) {
        bnds2D = true;
    }
    // inquire fields
    for (int i = 0; i < this->fieldInfos.size(); ++i) {
        string name = this->fieldInfos[i].field->name();
        ret = nc_inq_varid(this->fileID, name.c_str(), &this->fieldInfos[i].varID);
        CHECK_NC_INQ_VARID(ret, this->fileName, name);
        ret = nc_inq_vartype(this->fileID, this->fieldInfos[i].varID, &this->fieldInfos[i].xtype);
        CHECK_NC_INQ_VARTYPE(ret, this->fileName, name);
    }
}

template <class MeshType>
void StructuredDataFile<MeshType>::create(const TimeManager &timeManager) {
    const auto &domain = this->mesh().domain();
    string name, longName, units;
    int ret;
    // define spatial dimensions and their coordinate variables
    for (int m = 0; m < domain.numDim(); ++m) {
        name = domain.axisName(m);
        longName = domain.axisLongName(m);
        // full grids
        ret = nc_def_dim(this->fileID, name.c_str(),
                         this->mesh().numGrid(m, GridType::FULL), &fullDimIDs[m]);
        CHECK_NC_DEF_DIM(ret, this->fileName, name);
        ret = nc_def_var(this->fileID, name.c_str(), NC_DOUBLE, 1,
                         &fullDimIDs[m], &fullVarIDs[m]);
        CHECK_NC_DEF_VAR(ret, this->fileName, name);
        ret = nc_put_att(this->fileID, fullVarIDs[m], "long_name", NC_CHAR,
                         longName.length(), longName.c_str());
        CHECK_NC_PUT_ATT(ret, this->fileName, name, "long_name");
        // half grids
        name += "_bnds";
        ret = nc_def_dim(this->fileID, name.c_str(),
                         this->mesh().numGrid(m, GridType::HALF), &halfDimIDs[m]);
        CHECK_NC_DEF_DIM(ret, this->fileName, name);
        ret = nc_def_var(this->fileID, name.c_str(), NC_DOUBLE, 1,
                         &halfDimIDs[m], &halfVarIDs[m]);
        CHECK_NC_DEF_VAR(ret, this->fileName, name);
    }
    // define fields
    for (int i = 0; i < this->fieldInfos.size(); ++i) {
        name = this->fieldInfos[i].field->name();
        longName = this->fieldInfos[i].field->longName();
        units = this->fieldInfos[i].field->units();
        vector<int> dimIDs;
        int l = 1;
        switch (this->fieldInfos[i].field->staggerLocation()) {
            case Location::CENTER:
                switch (this->fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.numDim()+1);
                        dimIDs[0] = this->timeDimID;
                        for (int m = domain.numDim()-1; m >= 0; --m) {
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
                switch (this->fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.numDim()+1);
                        dimIDs[0] = this->timeDimID;
                        for (int m = domain.numDim()-1; m >= 0; --m) {
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
                switch (this->fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.numDim()+1);
                        dimIDs[0] = this->timeDimID;
                        for (int m = domain.numDim()-1; m >= 0; --m) {
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
                switch (this->fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.numDim()+1);
                        dimIDs[0] = this->timeDimID;
                        for (int m = domain.numDim()-1; m >= 0; --m) {
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
                switch (this->fieldInfos[i].spaceDims) {
                    case SpaceDimensions::FULL_DIMENSION:
                        dimIDs.resize(domain.numDim()+1);
                        dimIDs[0] = this->timeDimID;
                        for (int m = domain.numDim()-1; m >= 0; --m) {
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
        ret = nc_def_var(this->fileID, name.c_str(), this->fieldInfos[i].xtype,
                         dimIDs.size(), &dimIDs[0], &this->fieldInfos[i].varID);
        CHECK_NC_DEF_VAR(ret, this->fileName, name);
        ret = nc_put_att(this->fileID, this->fieldInfos[i].varID, "long_name", NC_CHAR,
                         longName.length(), longName.c_str());
        CHECK_NC_PUT_ATT(ret, this->fileName, name, "long_name");
        ret = nc_put_att(this->fileID, this->fieldInfos[i].varID, "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, this->fileName, name, "units");
    }
    // turn off define mode
    ret = nc_enddef(this->fileID);
}

template <class MeshType>
void StructuredDataFile<MeshType>::inputGrids() {
    REPORT_ERROR("Under construction!");
}

template <class MeshType>
void StructuredDataFile<MeshType>::inputHorizontalGrids() {
    REPORT_ERROR("Under construction!");
}

template <class MeshType>
void StructuredDataFile<MeshType>::inputVerticalGrids() {
    REPORT_ERROR("Under construction!");
}

template <class MeshType>
void StructuredDataFile<MeshType>::outputGrids() {
    const auto &domain = this->mesh().domain();
    int ret;
    // write units
    ret = nc_redef(this->fileID);
    for (int m = 0; m < domain.numDim(); ++m) {
        string units = domain.axisUnits(m);
        ret = nc_put_att(this->fileID, fullVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, this->fileName, domain.axisName(m), "units");
        ret = nc_put_att(this->fileID, halfVarIDs[m], "units", NC_CHAR,
                         units.length(), units.c_str());
        CHECK_NC_PUT_ATT(ret, this->fileName, domain.axisName(m)+"_bnds", "units");
    }
    ret = nc_enddef(this->fileID);
    // write spatial grids
    for (int m = 0; m < domain.numDim(); ++m) {
        ret = nc_put_var(this->fileID, fullVarIDs[m],
                         this->mesh().gridCoordComps(m, GridType::FULL).memptr());
        CHECK_NC_PUT_VAR(ret, this->fileName, domain.axisName(m));
        ret = nc_put_var(this->fileID, halfVarIDs[m],
                         this->mesh().gridCoordComps(m, GridType::HALF).memptr());
        CHECK_NC_PUT_VAR(ret, this->fileName, domain.axisName(m)+"_bnds");
    }
}

template <class MeshType>
void StructuredDataFile<MeshType>::registerField(const string &xtype, int spaceDims,
                                                 initializer_list<Field<MeshType>*> fields) {
    for (auto field : fields) {
        FieldInfo<MeshType> info;
        info.field = field;
        if (xtype == "double") {
            info.xtype = NC_DOUBLE;
        } else if (xtype == "float") {
            info.xtype = NC_FLOAT;
        } else if (xtype == "int") {
            info.xtype = NC_INT;
        } else {
            REPORT_ERROR("Unsupported data type \"" << xtype << "\"!");
        }
        info.spaceDims = spaceDims;
        this->fieldInfos.push_back(info);
    }
}

template <class MeshType>
void StructuredDataFile<MeshType>::removeField(initializer_list<Field<MeshType>*> fields) {
    for (auto field : fields) {
        int i;
        for (i = 0; i < this->fieldInfos.size(); ++i) {
            if (this->fieldInfos[i].field == field) {
                this->fieldInfos.erase(this->fieldInfos.begin()+i);
                break;
            }
        }
        if (i == this->fieldInfos.size()) {
            REPORT_ERROR("Field \"" << field->name() << "\" is not in the file \"" << this->fileName << "\"!");
        }
    }
}

template <class MeshType>
template <typename DataType, int NumTimeLevel>
void StructuredDataFile<MeshType>::input(const TimeLevelIndex<NumTimeLevel> &timeIdx,
                               			 initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, NumTimeLevel> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                // TODO: Try to eliminiate the following duplicates.
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_var_double(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_var_float(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_var_int(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for input!");
        }
    }
}

template <class MeshType>
template <typename DataType>
void StructuredDataFile<MeshType>::input(initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, 1> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_var_double(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_var_float(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_var_int(this->fileID, info.varID, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for input!");
        }
    }
}

template <class MeshType>
template <typename DataType, int NumTimeLevel>
void StructuredDataFile<MeshType>::input(const TimeLevelIndex<NumTimeLevel> &timeIdx,
                                         int timeCounter, initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, NumTimeLevel> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                size_t start[field->numDim()+1], count[field->numDim()+1];
                // assume the first dimension is time
                start[0] = timeCounter; count[0] = 1;
                if (field->numDim() == 2) {
                    start[1] = 0; start[2] = 0;
                    switch (field->staggerLocation()) {
                        case StructuredStagger::Location::CENTER:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::X_FACE:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::Y_FACE:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        case StructuredStagger::Location::XY_VERTEX:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        default:
                            REPORT_ERROR("Unexpected branch!");
                    }
                } else if (field->numDim() == 3) {
                    start[1] = 0; start[2] = 0; start[3] = 0;
                    switch (field->staggerLocation()) {
                        case StructuredStagger::Location::CENTER:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::X_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::Y_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        case StructuredStagger::Location::Z_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::XY_VERTEX:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        default:
                            REPORT_ERROR("Unexpected branch!");
                    }
                }
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_vara_double(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_vara_float(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_vara_int(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for input!");
        }
    }
}

template <class MeshType>
template <typename DataType>
void StructuredDataFile<MeshType>::input(int timeCounter, initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, 1> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                size_t start[field->numDim()+1], count[field->numDim()+1];
                // assume the first dimension is time
                start[0] = timeCounter; count[0] = 1;
                if (field->numDim() == 2) {
                    start[1] = 0; start[2] = 0;
                    switch (field->staggerLocation()) {
                        case StructuredStagger::Location::CENTER:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::X_FACE:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::Y_FACE:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        case StructuredStagger::Location::XY_VERTEX:
                            count[1] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        default:
                            REPORT_ERROR("Unexpected branch!");
                    }
                } else if (field->numDim() == 3) {
                    start[1] = 0; start[2] = 0; start[3] = 0;
                    switch (field->staggerLocation()) {
                        case StructuredStagger::Location::CENTER:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::X_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::Y_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        case StructuredStagger::Location::Z_FACE:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::HALF);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::FULL);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::FULL);
                            break;
                        case StructuredStagger::Location::XY_VERTEX:
                            count[1] = this->mesh().numGrid(2, StructuredStagger::GridType::FULL);
                            count[2] = this->mesh().numGrid(1, StructuredStagger::GridType::HALF);
                            count[3] = this->mesh().numGrid(0, StructuredStagger::GridType::HALF);
                            break;
                        default:
                            REPORT_ERROR("Unexpected branch!");
                    }
                }
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_vara_double(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_vara_float(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_vara_int(this->fileID, info.varID, start, count, x);
                    CHECK_NC_GET_VAR(ret, this->fileName, field->name());
                    for (int k = 0; k < n; ++k) {
                        (*field)(k) = x[k];
                    }
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for input!");
        }
    }
}

template <class MeshType>
template <typename DataType, int NumTimeLevel>
void StructuredDataFile<MeshType>::output(const TimeLevelIndex<NumTimeLevel> &timeIdx,
                                		  initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, NumTimeLevel> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(timeIdx, k);
                    }
                    ret = nc_put_var_double(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(timeIdx, k);
                    }
                    ret = nc_put_var_float(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(timeIdx, k);
                    }
                    ret = nc_put_var_int(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for output!");
        }
    }
}

template <class MeshType>
template <typename DataType>
void StructuredDataFile<MeshType>::output(initializer_list<Field<MeshType>*> fields) {
    typedef StructuredField<MeshType, DataType, 1> FieldType;
    int ret;
    for (auto field_ : fields) {
        bool tag = false;
        for (auto info : this->fieldInfos) {
            if (info.field == field_) {
                tag = true;
                FieldType *field = dynamic_cast<FieldType*>(field_);
                if (field == NULL) {
                    REPORT_ERROR("Field \"" << field_->name() << "\" does not match expected type!");
                }
                int n = this->mesh().totalNumGrid(field->staggerLocation(), field->numDim());
                if (info.xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(k);
                    }
                    ret = nc_put_var_double(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                } else if (info.xtype == NC_FLOAT) {
                    float *x = new float[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(k);
                    }
                    ret = nc_put_var_float(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                } else if (info.xtype == NC_INT) {
                    int *x = new int[n];
                    for (int k = 0; k < n; ++k) {
                        x[k] = (*field)(k);
                    }
                    ret = nc_put_var_int(this->fileID, info.varID, x);
                    CHECK_NC_PUT_VAR(ret, this->fileName, field->name());
                    delete [] x;
                }
                break;
            }
        }
        if (!tag) {
            REPORT_ERROR("Field \"" << field_->name() << "\" is not registered for input!");
        }
    }
}

} // geomtk