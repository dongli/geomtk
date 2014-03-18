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

enum IOFrequencyUnit {
    STEPS, SECONDS, MINUTES, HOURS, DAYS, MONTHS, YEARS
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
    IOFrequencyUnit freqUnit;
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
    const StructuredMesh *mesh;
protected:
    vector<int> fullDimIDs, fullVarIDs;
    vector<int> halfDimIDs, halfVarIDs;
public:
    typedef StructuredMesh MeshType;

    StructuredDataFile(const StructuredMesh &mesh);
    virtual ~StructuredDataFile() {}

    virtual const StructuredMesh& getMesh() const { return *mesh; }

    template <class FieldElementType, int numLevel, int spaceDims>
    void registerOutputField(initializer_list<Field*> fields);

    virtual void create(const TimeManager &timeManager);

    virtual void outputGrids();

    template <class FieldElementType, int numLevel>
    void output(const TimeLevelIndex<numLevel> &timeIdx,
                initializer_list<Field*> fields);
};

template <class FieldElementType, int numLevel, int spaceDims>
void StructuredDataFile::registerOutputField(initializer_list<Field*> fields) {
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

template <class FieldElementType, int numLevel>
void StructuredDataFile::output(const TimeLevelIndex<numLevel> &timeIdx,
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

// -----------------------------------------------------------------------------

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

    virtual void outputGrids();
};

// -----------------------------------------------------------------------------

template <class DataFileType>
class IOManager {
private:
    const TimeManager *timeManager;
    vector<DataFileType> files;
public:
    IOManager();
    virtual ~IOManager();

    void init(const TimeManager &timeManager);

    int registerOutputFile(const typename DataFileType::MeshType &mesh,
                           const string &prefix, IOFrequencyUnit freqUnit,
                           double freq);

    DataFileType& file(int fileIdx) { return files[fileIdx]; }

    void create(int fileIdx);

    template <class FieldElementType, int numLevel>
    void output(int fileIdx, const TimeLevelIndex<numLevel> &timeIdx,
                initializer_list<Field*> fields);

    void close(int fileIdx);
private:
    void checkFileActive(DataFileType &dataFile) const;
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
int IOManager<DataFileType>::registerOutputFile(const typename DataFileType::MeshType &mesh,
                                                const string &prefix,
                                                IOFrequencyUnit freqUnit,
                                                double freq) {
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
    file.freqUnit = freqUnit;
    file.freq = freq;
    file.isActive = false;
    file.lastTime = -1;
    files.push_back(file);
    REPORT_NOTICE("Register output file with pattern " << filePattern << ".");
    return files.size()-1;
}
    
template <class DataFileType>
void IOManager<DataFileType>::checkFileActive(DataFileType &dataFile) const {
    double time;
    switch (dataFile.freqUnit) {
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
    }
    if (dataFile.lastTime == -1 || time-dataFile.lastTime >= dataFile.freq) {
        dataFile.lastTime = time;
        dataFile.isActive = true;
    }
}

template <class DataFileType>
void IOManager<DataFileType>::create(int fileIdx) {
    DataFileType &dataFile = files[fileIdx];

    // check if it is time to output
    checkFileActive(dataFile);
    if (!dataFile.isActive) return;

    dataFile.fileName = dataFile.filePattern.run("%5.5d", timeManager->getNumStep());
    int ret = nc_create(dataFile.fileName.c_str(), NC_CLOBBER, &dataFile.fileID);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Unable to create file \"" << dataFile.fileName <<
                     "\" with error message \"" << nc_strerror(ret) << "\"!");
    }
    // let concrete data file class create the rest data file
    dataFile.create(*timeManager);
    dataFile.outputGrids();
}

template <class DataFileType>
template <class FieldElementType, int numLevel>
void IOManager<DataFileType>::output(int fileIdx,
                                     const TimeLevelIndex<numLevel> &timeIdx,
                                     initializer_list<Field*> fields) {
    DataFileType &dataFile = files[fileIdx];
    if (!dataFile.isActive) return;
    // write time
    double time = timeManager->getDays();
    size_t index[1] = {0};
    int ret = nc_put_var1(dataFile.fileID, dataFile.timeVarID, index, &time);
    if (ret != NC_NOERR) {
        REPORT_ERROR("Failed to put variable time with error message \"" <<
                     nc_strerror(ret) << "\"!");
    }
    // write fields
    dataFile.template output<FieldElementType, numLevel>(timeIdx, fields);
}

template <class DataFileType>
void IOManager<DataFileType>::close(int fileIdx) {
    DataFileType &dataFile = files[fileIdx];
    if (!dataFile.isActive) return;
    if (nc_close(dataFile.fileID) != NC_NOERR) {
        REPORT_ERROR("Failed to close output file \"" <<
                     files[fileIdx].fileName << "\"!");
    }
    dataFile.isActive = false;
}

} // geomtk

#endif // __Geomtk_IOManager__