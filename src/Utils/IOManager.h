#ifndef __Geomtk_IOManager__
#define __Geomtk_IOManager__

#include "geomtk_commons.h"
#include "StructuredField.h"
#include "RLLField.h"
#include "TimeManager.h"
#include "StampString.h"

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

struct IOFrequencyUnit {
    static const int STEPS = 0;
    static const int SECONDS = 1;
    static const int MINUTES = 2;
    static const int HOURS = 3;
    static const int DAYS = 4;
    static const int MONTHS = 5;
    static const int YEARS = 6;
};

// -----------------------------------------------------------------------------

class DataFile {
public:
    StampString filePattern;
    string fileName;
    int fileID;
    int timeDimID, timeVarID;
    vector<FieldInfo> fieldInfos;
    IOType ioType;
    int freqUnit;
    double freq;
    double lastTime;
    bool isActive;
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
    StructuredMesh *mesh;
protected:
    vector<int> fullDimIDs, fullVarIDs;
    vector<int> halfDimIDs, halfVarIDs;
public:
    typedef StructuredMesh MeshType;

    StructuredDataFile(StructuredMesh &mesh);
    virtual ~StructuredDataFile() {}

    virtual const StructuredMesh& getMesh() const { return *mesh; }

    template <typename FieldElementType, int numLevel, int spaceDims>
    void registerInputField(int numField, ...);

    template <typename FieldElementType, int numLevel, int spaceDims>
    void registerOutputField(int numField, ...);
    
    void removeOutputField(int numField, ...);

    virtual void open(const TimeManager &timeManager);

    virtual void create(const TimeManager &timeManager);

    virtual void inputGrids();

    virtual void outputGrids();

    template <typename FieldElementType, int numLevel>
    void input(const TimeLevelIndex<numLevel> &timeIdx,
               int numField, va_list fields);

    template <typename FieldElementType, int numLevel>
    void input(int numField, va_list fields);

    template <typename FieldElementType, int numLevel>
    void output(const TimeLevelIndex<numLevel> &timeIdx,
                int numField, va_list fields);
    
    template <typename FieldElementType, int numLevel>
    void output(int numField, va_list fields);
};
    
template <typename FieldElementType, int numLevel, int spaceDims>
void StructuredDataFile::registerInputField(int numField, ...) {
    va_list fields;
    va_start(fields, numField);
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        FieldInfo info;
        info.field = field;
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
    va_end(fields);
}

template <typename FieldElementType, int numLevel, int spaceDims>
void StructuredDataFile::registerOutputField(int numField, ...) {
    va_list fields;
    va_start(fields, numField);
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        FieldInfo info;
        info.field = field;
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
    va_end(fields);
}

template <typename FieldElementType, int numLevel>
void StructuredDataFile::input(const TimeLevelIndex<numLevel> &timeIdx,
                               int numField, va_list fields) {
    typedef StructuredField<FieldElementType, numLevel> FieldType;
    int ret;
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        FieldType *f = dynamic_cast<FieldType*>(field);
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        int j;
        for (j = 0; j < fieldInfos.size(); ++j) {
            if (fieldInfos[j].field == field) {
                if (f == NULL) {
                    REPORT_ERROR("Field type is not correct!");
                }
                int n = mesh->getTotalNumGrid(f->getStaggerLocation());
                if (fieldInfos[j].xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_var_int(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (fieldInfos[j].xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_var_float(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else if (fieldInfos[j].xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_var_double(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(timeIdx, k) = x[k];
                    }
                    delete [] x;
                } else {
                    REPORT_ERROR("Unsupported variable type!");
                }
                break;
            }
        }
        if (j == fieldInfos.size()) {
            REPORT_ERROR("Field \"" << f->getName() << "\" is not registered for input!");
        }
    }
}
    
template <typename FieldElementType, int numLevel>
void StructuredDataFile::input(int numField, va_list fields) {
    typedef StructuredField<FieldElementType, numLevel> FieldType;
    int ret;
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        FieldType *f = dynamic_cast<FieldType*>(field);
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        int j;
        for (j = 0; j < fieldInfos.size(); ++j) {
            if (fieldInfos[j].field == field) {
                if (f == NULL) {
                    REPORT_ERROR("Field type is not correct!");
                }
                int n = mesh->getTotalNumGrid(f->getStaggerLocation());
                if (fieldInfos[j].xtype == NC_INT) {
                    int *x = new int[n];
                    ret = nc_get_var_int(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(k) = x[k];
                    }
                    delete [] x;
                } else if (fieldInfos[j].xtype == NC_FLOAT) {
                    float *x = new float[n];
                    ret = nc_get_var_float(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(k) = x[k];
                    }
                    delete [] x;
                } else if (fieldInfos[j].xtype == NC_DOUBLE) {
                    double *x = new double[n];
                    ret = nc_get_var_double(fileID, fieldInfos[j].varID, x);
                    if (ret != NC_NOERR) {
                        REPORT_ERROR("Failed to get variable \"" << f->getName() <<
                                     "\" with error message \"" << nc_strerror(ret) <<
                                     "\" in file \"" << fileName << "\"!");
                    }
                    for (int k = 0; k < n; ++k) {
                        (*f)(k) = x[k];
                    }
                    delete [] x;
                } else {
                    REPORT_ERROR("Unsupported variable type!");
                }
                break;
            }
        }
        if (j == fieldInfos.size()) {
            REPORT_ERROR("Field \"" << f->getName() << "\" is not registered for input!");
        }
    }
}

template <typename FieldElementType, int numLevel>
void StructuredDataFile::output(const TimeLevelIndex<numLevel> &timeIdx,
                                int numField, va_list fields) {
    typedef StructuredField<FieldElementType, numLevel> FieldType;
    int ret;
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        const FieldType *f = dynamic_cast<const FieldType*>(field);
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        int j;
        for (j = 0; j < fieldInfos.size(); ++j) {
            if (fieldInfos[j].field == field) {
                if (f == NULL) {
                    REPORT_ERROR("Field type is not correct!");
                }
                int n = mesh->getTotalNumGrid(f->getStaggerLocation());
                double *x = new double[n];
                for (int k = 0; k < n; ++k) {
                    x[k] = (*f)(timeIdx, k);
                }
                ret = nc_put_var(fileID, fieldInfos[j].varID, x);
                if (ret != NC_NOERR) {
                    REPORT_ERROR("Failed to put variable \"" << f->getName() <<
                                 "\" with error message \"" << nc_strerror(ret) <<
                                 "\" in file \"" << fileName << "\"!");
                }
                delete [] x;
                break;
            }
        }
        if (j == fieldInfos.size()) {
            REPORT_ERROR("Field \"" << f->getName() << "\" is not registered for output!");
        }
    }
}
    
template <typename FieldElementType, int numLevel>
void StructuredDataFile::output(int numField, va_list fields) {
    typedef StructuredField<FieldElementType, numLevel> FieldType;
    int ret;
    for (int i = 0; i < numField; ++i) {
        Field *field = dynamic_cast<Field*>(va_arg(fields, Field*));
        const FieldType *f = dynamic_cast<const FieldType*>(field);
        if (field == NULL) {
            REPORT_ERROR("Argument " << i+2 << " is not a Field pointer!");
        }
        int j;
        for (j = 0; j < fieldInfos.size(); ++j) {
            if (fieldInfos[j].field == field) {
                if (f == NULL) {
                    REPORT_ERROR("Field type is not correct!");
                }
                int n = mesh->getTotalNumGrid(f->getStaggerLocation());
                double *x = new double[n];
                for (int k = 0; k < n; ++k) {
                    x[k] = (*f)(k);
                }
                ret = nc_put_var(fileID, fieldInfos[j].varID, x);
                if (ret != NC_NOERR) {
                    REPORT_ERROR("Failed to put variable \"" << f->getName() <<
                                 "\" with error message \"" << nc_strerror(ret) <<
                                 "\" in file \"" << fileName << "\"!");
                }
                delete [] x;
                break;
            }
        }
        if (j == fieldInfos.size()) {
            REPORT_ERROR("Field \"" << f->getName() << "\" is not registered for output!");
        }
    }
}

// -----------------------------------------------------------------------------

struct RLLSpaceDimensions : public StructuredSpaceDimensions {
};

class RLLDataFile : public StructuredDataFile {
    RLLMesh *mesh;
public:
    typedef RLLMesh MeshType;

    RLLDataFile(RLLMesh &mesh) : StructuredDataFile(mesh) {
        this->mesh = &mesh;
    }
    virtual ~RLLDataFile() {}

    virtual const RLLMesh& getMesh() const { return *mesh; }

    virtual void inputGrids();

    virtual void outputGrids();
};

// -----------------------------------------------------------------------------
/**
 *  This class manages the I/O for fields.
 */
template <class DataFileType>
class IOManager {
private:
    const TimeManager *timeManager;
    vector<DataFileType> files;
public:
    IOManager();
    virtual ~IOManager();

    void init(const TimeManager &timeManager);

    int registerInputFile(typename DataFileType::MeshType &mesh,
                          const string &fileName);

    int registerOutputFile(typename DataFileType::MeshType &mesh,
                           const string &prefix, int freqUnit, double freq);

    void removeFile(int fileIdx);

    DataFileType& file(int fileIdx);

    void open(int fileIdx);

    void create(int fileIdx);

    void updateTime(int fileIdx, TimeManager &timeManager);

    template <typename FieldElementType, int numLevel>
    void input(int fileIdx, const TimeLevelIndex<numLevel> &timeIdx,
               int numField, ...);

    template <typename FieldElementType, int numLevel>
    void input(int fileIdx, int numField, ...);

    template <typename FieldElementType, int numLevel>
    void output(int fileIdx, const TimeLevelIndex<numLevel> &timeIdx,
                int numField, ...);
    
    template <typename FieldElementType, int numLevel>
    void output(int fileIdx, int numField, ...);

    void close(int fileIdx);

    /**
     *  Check if file is active for input or output.
     *
     *  @param file the data file object.
     */
    void checkFileActive(DataFileType &file);

    /**
     *  Check if file is active for input or output.
     *
     *  @param fileIdx the data file index.
     *
     *  @return The boolean result.
     */
    bool isFileActive(int fileIdx);
};
    
template <class DataFileType>
IOManager<DataFileType>::IOManager() {
    REPORT_ONLINE;
}

template <class DataFileType>
IOManager<DataFileType>::~IOManager() {
    REPORT_OFFLINE;
}

template <class DataFileType>
void IOManager<DataFileType>::init(const TimeManager &timeManager) {
    this->timeManager = &timeManager;
}

template <class DataFileType>
int IOManager<DataFileType>::registerInputFile(typename DataFileType::MeshType &mesh,
                                               const string &fileName) {
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].ioType == INPUT && files[i].fileName == fileName) {
            REPORT_ERROR("File with file name " << fileName <<
                         " has already been registered for input!");
        }
    }
    DataFileType file(mesh);
    file.fileName = fileName;
    file.ioType = INPUT;
    file.isActive = true;
    files.push_back(file);
    REPORT_NOTICE("Register input file with name " << fileName << ".");
    return files.size()-1;
}
    
template <class DataFileType>
int IOManager<DataFileType>::registerOutputFile(typename DataFileType::MeshType &mesh,
                                                const string &prefix, int freqUnit, double freq) {
    StampString filePattern(prefix+".", ".nc");
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].ioType == OUTPUT && files[i].filePattern == filePattern) {
            REPORT_ERROR("File with pattern " << filePattern <<
                         " has already been registered for output!");
        }
    }
    DataFileType file(mesh);
    file.filePattern = filePattern;
    file.ioType = OUTPUT;
    file.freqUnit = freqUnit;
    file.freq = freq;
    file.isActive = false;
    file.lastTime = -1;
    files.push_back(file);
    REPORT_NOTICE("Register output file with pattern " << filePattern << ".");
    return files.size()-1;
}

template <class DataFileType>
void IOManager<DataFileType>::removeFile(int fileIdx) {
    if (fileIdx < 0 || fileIdx >= files.size()) {
        REPORT_ERROR("File index is out of range!");
    }
    files.erase(files.begin()+fileIdx);
}

template <class DataFileType>
DataFileType& IOManager<DataFileType>::file(int fileIdx) {
    if (fileIdx < 0 || fileIdx >= files.size()) {
        REPORT_ERROR("File index is out of range!");
    }
    return files[fileIdx];
}
    
template <class DataFileType>
void IOManager<DataFileType>::checkFileActive(DataFileType &file) {
    double time;
    switch (file.freqUnit) {
        case IOFrequencyUnit::STEPS:
            time = timeManager->getNumStep();
            break;
        case IOFrequencyUnit::SECONDS:
            time = timeManager->getSeconds();
            break;
        case IOFrequencyUnit::MINUTES:
            time = timeManager->getMinutes();
            break;
        case IOFrequencyUnit::HOURS:
            time = timeManager->getHours();
            break;
        case IOFrequencyUnit::DAYS:
            time = timeManager->getDays();
            break;
        case IOFrequencyUnit::MONTHS:
            REPORT_ERROR("Under construction!");
            break;
        case IOFrequencyUnit::YEARS:
            REPORT_ERROR("Under construction!");
            break;
        default:
            REPORT_ERROR("Unknown IO frequency unit!");
    }
    if (file.lastTime == -1 || time-file.lastTime >= file.freq) {
        file.lastTime = time;
        file.isActive = true;
    }
}

template <class DataFileType>
bool IOManager<DataFileType>::isFileActive(int fileIdx) {
    DataFileType &file = files[fileIdx];
    checkFileActive(file);
    return file.isActive;
}

template <class DataFileType>
void IOManager<DataFileType>::open(int fileIdx) {
    DataFileType &file = files[fileIdx];
    
    int ret;
    ret = nc_open(file.fileName.c_str(), NC_NOWRITE, &file.fileID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Unable to open file \"" << file.fileName <<
                     "\" with error message \"" << nc_strerror(ret) << "\"!");
    }
    ret = nc_inq_varid(file.fileID, "time", &file.timeVarID);
    if (ret != NC_NOERR) {
        file.timeVarID = -999;
    }
    // let concrete data file class open the rest data file
    file.open(*timeManager);
}
    
template <class DataFileType>
void IOManager<DataFileType>::create(int fileIdx) {
    DataFileType &file = files[fileIdx];

    // check if it is time to output
    checkFileActive(file);
    if (!file.isActive) return;

    file.fileName = file.filePattern.run("%5.5d", timeManager->getNumStep());
    int ret = nc_create(file.fileName.c_str(), NC_CLOBBER, &file.fileID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Unable to create file \"" << file.fileName <<
                     "\" with error message \"" << nc_strerror(ret) << "\"!");
    }
    // let concrete data file class create the rest data file
    file.create(*timeManager);
    file.outputGrids();
}

template <class DataFileType>
void IOManager<DataFileType>::updateTime(int fileIdx, TimeManager &timeManager) {
    DataFileType &file = files[fileIdx];
    double value;
    char units[100];
    int ret;
    ret = nc_get_var(file.fileID, file.timeVarID, &value);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to get time variable in file \"" << file.fileName <<
                     "\" with error message \"" << nc_strerror(ret) << "\"!");
    }
    ret = nc_get_att_text(file.fileID, file.timeVarID, "units", units);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to get time units in file \"" << file.fileName <<
                     "\" with error message \"" << nc_strerror(ret) << "\"!");
    }
    regex reDays("^days");
    regex reDate("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d) (\\d\\d)*:(\\d\\d)*:(\\d\\.\\d*)*");
    match_results<std::string::const_iterator> what;
    string tmp = units;
    Time time;
    if (regex_search(tmp, what, reDate)) {
        tmp = what[1]; time.year = atoi(tmp.c_str());
        tmp = what[2]; time.month = atoi(tmp.c_str());
        tmp = what[3]; time.day = atoi(tmp.c_str());
        tmp = what[4]; time.hour = atoi(tmp.c_str());
        tmp = what[5]; time.minute = atoi(tmp.c_str());
        tmp = what[6]; time.second = atof(tmp.c_str());
    }
    if (regex_search(units, reDays)) {
        time += value*TimeUnit::DAYS;
    } else {
        REPORT_ERROR("Unsupported time units!");
    }
    timeManager.resetCurrentTime(time);
}

template <class DataFileType>
template <typename FieldElementType, int numLevel>
void IOManager<DataFileType>::input(int fileIdx,
                                    const TimeLevelIndex<numLevel> &timeIdx,
                                    int numField, ...) {
    DataFileType &file = files[fileIdx];
    va_list fields;
    va_start(fields, numField);
    file.template input<FieldElementType, numLevel>(timeIdx, numField, fields);
    va_end(fields);
}

template <class DataFileType>
template <typename FieldElementType, int numLevel>
void IOManager<DataFileType>::input(int fileIdx, int numField, ...) {
    DataFileType &file = files[fileIdx];
    va_list fields;
    va_start(fields, numField);
    file.template input<FieldElementType, numLevel>(numField, fields);
    va_end(fields);
}

template <class DataFileType>
template <typename FieldElementType, int numLevel>
void IOManager<DataFileType>::output(int fileIdx,
                                     const TimeLevelIndex<numLevel> &timeIdx,
                                     int numField, ...) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    // write time
    double time = timeManager->getDays();
    size_t index[1] = {0};
    int ret = nc_put_var1(file.fileID, file.timeVarID, index, &time);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to put variable time with error message \"" <<
                     nc_strerror(ret) << "\"!");
    }
    // write fields
    va_list fields;
    va_start(fields, numField);
    file.template output<FieldElementType, numLevel>(timeIdx, numField, fields);
    va_end(fields);
}
    
template <class DataFileType>
template <typename FieldElementType, int numLevel>
void IOManager<DataFileType>::output(int fileIdx, int numField, ...) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
        // write time
    double time = timeManager->getDays();
    size_t index[1] = {0};
    int ret = nc_put_var1(file.fileID, file.timeVarID, index, &time);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to put variable time with error message \"" <<
                     nc_strerror(ret) << "\"!");
    }
    // write fields
    va_list fields;
    va_start(fields, numField);
    file.template output<FieldElementType, numLevel>(numField, fields);
    va_end(fields);
}

template <class DataFileType>
void IOManager<DataFileType>::close(int fileIdx) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    if (nc_close(file.fileID) != NC_NOERR) {
        REPORT_ERROR("Failed to close output file \"" <<
                     files[fileIdx].fileName << "\"!");
    }
    file.isActive = false;
}

} // geomtk

#endif // __Geomtk_IOManager__
