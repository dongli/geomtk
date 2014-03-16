#ifndef __Geomtk_IOManager__
#define __Geomtk_IOManager__

#include "geomtk_commons.h"
#include "StructuredField.h"
#include "RLLField.h"
#include "TimeManager.h"

namespace geomtk {

struct FieldInfo {
    Field *field;
    nc_type xtype;
    int varID;
    int spaceDims;
};

enum IOType {
    INPUT, OUTPUT
};

// -----------------------------------------------------------------------------

class DataFile {
public:
    StampString filePattern;
    string fileName;
    int fileID;
    int timeDimID, timeVarID;
    list<FieldInfo> fieldInfos;
    IOType ioType;
public:
    DataFile() {}
    virtual ~DataFile() {}
};

struct StructuredSpaceDimensions {
    static const int FULL_DIMENSION = 0;
    static const int XY_DIMENSION = 1;
    static const int YZ_DIMENSION = 2;
    static const int XZ_DIMENSION = 3;
};

class StructuredDataFile : public DataFile {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
    typedef StructuredSpaceDimensions SpaceDimensions;
private:
    const StructuredMesh *mesh;
protected:
    vector<int> fullDimIDs, fullVarIDs;
    vector<int> halfDimIDs, halfVarIDs;
public:
    typedef StructuredMesh MeshType;

    StructuredDataFile(const StructuredMesh &mesh) : DataFile() {
        this->mesh = &mesh;
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            fullDimIDs.resize(mesh.getNumGrid(m, GridType::FULL));
            fullVarIDs.resize(mesh.getNumGrid(m, GridType::FULL));
            halfDimIDs.resize(mesh.getNumGrid(m, GridType::HALF));
            halfVarIDs.resize(mesh.getNumGrid(m, GridType::HALF));
        }
    }
    virtual ~StructuredDataFile() {}

    virtual const StructuredMesh& getMesh() const { return *mesh; }

    template <class FieldElementType, int numLevel, int spaceDims>
    void registerOutputField(initializer_list<Field*> fields) {
        for (auto f : fields) {
            FieldInfo info;
            info.field = f;
            if (is_same<FieldElementType, double>::value) {
                info.xtype = NC_DOUBLE;
            } else if (is_same<FieldElementType, int>::value) {
                info.xtype = NC_INT;
            } else {
                REPORT_ERROR("Unsupported data type \"" <<
                             typeid(FieldElementType).name() << "\"!");
            }
            info.spaceDims = spaceDims;
            fieldInfos.push_back(info);
        }
    }

    virtual void create(const TimeManager &timeManager) {
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
        for (auto &info : fieldInfos) {
            name = info.field->getName();
            longName = info.field->getLongName();
            units = info.field->getUnits();
            vector<int> dimIDs;
            int i = 1;
            switch (info.field->getStaggerLocation()) {
                case Location::CENTER:
                    switch (info.spaceDims) {
                        case SpaceDimensions::FULL_DIMENSION:
                            dimIDs.resize(domain.getNumDim()+1);
                            dimIDs[0] = timeDimID;
                            for (int m = domain.getNumDim()-1; m >= 0; --m) {
                                dimIDs[i++] = fullDimIDs[m];
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
                    switch (info.spaceDims) {
                        case SpaceDimensions::FULL_DIMENSION:
                            dimIDs.resize(domain.getNumDim()+1);
                            dimIDs[0] = timeDimID;
                            for (int m = domain.getNumDim()-1; m >= 0; --m) {
                                if (m == 0) {
                                    dimIDs[i++] = halfDimIDs[m];
                                } else {
                                    dimIDs[i++] = fullDimIDs[m];
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
                    switch (info.spaceDims) {
                        case SpaceDimensions::FULL_DIMENSION:
                            dimIDs.resize(domain.getNumDim()+1);
                            dimIDs[0] = timeDimID;
                            for (int m = domain.getNumDim()-1; m >= 0; --m) {
                                if (m == 1) {
                                    dimIDs[i++] = halfDimIDs[m];
                                } else {
                                    dimIDs[i++] = fullDimIDs[m];
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
                    switch (info.spaceDims) {
                        case SpaceDimensions::FULL_DIMENSION:
                            dimIDs.resize(domain.getNumDim()+1);
                            dimIDs[0] = timeDimID;
                            for (int m = domain.getNumDim()-1; m >= 0; --m) {
                                if (m == 2) {
                                    dimIDs[i++] = halfDimIDs[m];
                                } else {
                                    dimIDs[i++] = fullDimIDs[m];
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
                    switch (info.spaceDims) {
                        case SpaceDimensions::FULL_DIMENSION:
                            dimIDs.resize(domain.getNumDim()+1);
                            dimIDs[0] = timeDimID;
                            for (int m = domain.getNumDim()-1; m >= 0; --m) {
                                if (m == 0 || m == 1) {
                                    dimIDs[i++] = halfDimIDs[m];
                                } else {
                                    dimIDs[i++] = fullDimIDs[m];
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
            ret = nc_def_var(fileID, name.c_str(), info.xtype,
                             dimIDs.size(), &dimIDs[0], &info.varID);
            if (ret != NC_NOERR) {
                REPORT_ERROR("Failed to define variable \"" << name <<
                             "\" with erro message \"" <<
                             nc_strerror(ret) << "\"!");
            }
            ret = nc_put_att(fileID, info.varID, "long_name", NC_CHAR,
                             longName.length(), longName.c_str());
            if (ret != NC_NOERR) {
                REPORT_ERROR("Failed to put \"long_name\" attribute to " <<
                             "variable \"" << name << "\" with error " <<
                             "message \"" << nc_strerror(ret) << "\" " <<
                             "in file \"" << fileName << "\"!");
            }
            ret = nc_put_att(fileID, info.varID, "units", NC_CHAR,
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

    virtual void outputGrids() {
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

    template <class FieldElementType, int numLevel>
    void output(const TimeLevelIndex<numLevel> &timeIdx,
                initializer_list<Field*> fields) {
        typedef StructuredField<FieldElementType, numLevel> FieldType;
        int ret;
        for (auto field : fields) {
            for (const auto &info : fieldInfos) {
                if (info.field == field) {
                    const FieldType *f = dynamic_cast<const FieldType*>(info.field);
                    if (f == NULL) {
                        REPORT_ERROR("Field type is not correct!");
                    }
                    int n = mesh->getTotalNumGrid(f->getStaggerLocation());
                    double *x = new double[n];
                    for (int i = 0; i < n; ++i) {
                        x[i] = (*f)(timeIdx, i);
                    }
                    ret = nc_put_var(fileID, info.varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to put variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    delete [] x;
                    break;
                }
            }
        }
    }
};

struct RLLSpaceDimensions : public StructuredSpaceDimensions {
};

class RLLDataFile : public StructuredDataFile {
    const RLLMesh *mesh;
public:
    typedef RLLMesh MeshType;

    RLLDataFile(const RLLMesh &mesh) : StructuredDataFile(mesh) {
        this->mesh = &mesh;
    }
    virtual ~RLLDataFile() {}

    virtual const RLLMesh& getMesh() const { return *mesh; }

    virtual void outputGrids() {
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
};

// -----------------------------------------------------------------------------

template <class DataFileType>
class IOManager {
    const TimeManager *timeManager;
    vector<DataFileType> files;
public:
    IOManager();
    virtual ~IOManager();

    void init(const TimeManager &timeManager) {
        this->timeManager = &timeManager;
    }

    int registerOutputFile(const typename DataFileType::MeshType &mesh,
                           const string &prefix) {
        StampString filePattern(prefix+".", ".nc");
        for (const auto &file : files) {
            if (file.filePattern == filePattern) {
                REPORT_ERROR("File with pattern " << filePattern <<
                             " has already been registered!");
            }
        }
        DataFileType file(mesh);
        file.filePattern = filePattern;
        file.ioType = OUTPUT;
        files.push_back(file);
        REPORT_NOTICE("Register output file with pattern " << filePattern << ".");
        return files.size()-1;
    }

    DataFileType& operator()(int fileIdx) {
        return files[fileIdx];
    }

    DataFileType create(int fileIdx) {
        DataFileType &dataFile = files[fileIdx];
        int ret;

        dataFile.fileName = dataFile.filePattern.run("%3.3d", timeManager->getNumStep());
        ret = nc_create(dataFile.fileName.c_str(), NC_CLOBBER, &dataFile.fileID);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Unable to create file \"" << dataFile.fileName <<
                         "\" with error message \"" << nc_strerror(ret) << "\"!");
        }

        dataFile.create(*timeManager);
        dataFile.outputGrids();

        return dataFile;
    }

    template <class FieldElementType, int numLevel>
    void output(int fileIdx, const TimeLevelIndex<numLevel> &timeIdx,
                initializer_list<Field*> fields) {
        DataFileType &dataFile = files[fileIdx];
        int ret;
        // write time
        double time = timeManager->getDays();
        size_t index[1] = {0};
        ret = nc_put_var1(dataFile.fileID, dataFile.timeVarID, index, &time);
        if (ret != NC_NOERR) {
            REPORT_ERROR("Failed to put variable time with error message \"" <<
                         nc_strerror(ret) << "\"!");
        }
        // write fields
        dataFile.template output<FieldElementType, numLevel>(timeIdx, fields);
    }

    void close(int fileIdx) {
        if (nc_close(files[fileIdx].fileID) != NC_NOERR) {
            REPORT_ERROR("Failed to close output file \"" <<
                         files[fileIdx].fileName << "\"!");
        }
    }
};
    
template <class DataFileType>
IOManager<DataFileType>::IOManager() {
    REPORT_ONLINE;
}

template <class DataFileType>
IOManager<DataFileType>::~IOManager() {
    REPORT_OFFLINE;
}

} // geomtk

#endif // __Geomtk_IOManager__