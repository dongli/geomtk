#include "IOManager.h"

namespace geomtk {
 
template <class DataFileType>
IOManager<DataFileType>::IOManager() {
}

template <class DataFileType>
IOManager<DataFileType>::~IOManager() {
}

template <class DataFileType>
void IOManager<DataFileType>::init(TimeManager &timeManager) {
    this->timeManager = &timeManager;
}

template <class DataFileType>
int IOManager<DataFileType>::registerInputFile(MeshType &mesh,
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
                                                StampString &filePattern,
                                                TimeStepUnit freqUnit, double freq) {
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].ioType == OUTPUT && files[i].filePattern == filePattern) {
            REPORT_ERROR("File with pattern \"" << filePattern <<
                         "\" has already been registered for output!");
        }
    }
    // add an alarm for output
    DataFileType file(mesh);
    file.filePattern = filePattern;
    file.ioType = OUTPUT;
    file.freqUnit = freqUnit;
    file.freq = freq;
    file.alarmIdx = timeManager->addAlarm(freqUnit, freq);
    file.isActive = false;
    file.lastTime = -1;
    files.push_back(file);
    REPORT_NOTICE("Register output file with pattern " << filePattern << ".");
    return files.size()-1;
}

template <class DataFileType>
int IOManager<DataFileType>::registerOutputFile(typename DataFileType::MeshType &mesh,
                                                const string &filePattern,
                                                TimeStepUnit freqUnit, double freq) {
    StampString tmp(filePattern);
    return registerOutputFile(mesh, tmp, freqUnit, freq);
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
bool IOManager<DataFileType>::isFileActive(int fileIdx) {
    DataFileType &file = files[fileIdx];
    file.isActive = timeManager->checkAlarm(file.alarmIdx);
    return file.isActive;
}

template <class DataFileType>
void IOManager<DataFileType>::open(int fileIdx) {
    DataFileType &file = files[fileIdx];
    int ret;
    ret = nc_open(file.fileName.c_str(), NC_NOWRITE, &file.fileID);
    CHECK_NC_OPEN(ret, file.fileName);
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
    if (!isFileActive(fileIdx)) return;
    file.fileName = file.filePattern.run(*timeManager);
    int ret = nc_create(file.fileName.c_str(), NC_CLOBBER, &file.fileID);
    CHECK_NC_CREATE(ret, file.fileName);
    // define temporal dimension
    int timeStep = timeManager->getNumStep();
    ret = nc_put_att(file.fileID, NC_GLOBAL, "time_step", NC_INT, 1, &timeStep);
    CHECK_NC_PUT_ATT(ret, file.fileName, "NC_GLOBAL", "time_step");
    ret = nc_def_dim(file.fileID, "time", NC_UNLIMITED, &file.timeDimID);
    CHECK_NC_DEF_DIM(ret, file.fileName, "time");
    ret = nc_def_var(file.fileID, "time", NC_DOUBLE, 1, &file.timeDimID, &file.timeVarID);
    CHECK_NC_DEF_VAR(ret, file.fileName, "time")
    string units = "days since "+timeManager->getStartTime().s();
    ret = nc_put_att(file.fileID, file.timeVarID, "units", NC_CHAR,
                     units.length(), units.c_str());
    CHECK_NC_PUT_ATT(ret, file.fileName, "time", "units");
    // let concrete data file class create the rest data file
    file.create(*timeManager);
    file.outputGrids();
}

template <class DataFileType>
void IOManager<DataFileType>::updateTime(int fileIdx, TimeManager &timeManager) {
    DataFileType &file = files[fileIdx];
    int timeStep;
    double timeValue;
    char units[100];
    int ret;
    ret = nc_get_att(file.fileID, NC_GLOBAL, "time_step", &timeStep);
    CHECK_NC_GET_ATT(ret, file.fileName, "NC_GLOBAL", "time_step");
    ret = nc_get_var(file.fileID, file.timeVarID, &timeValue);
    CHECK_NC_GET_VAR(ret, file.fileName, "time");
    ret = nc_get_att_text(file.fileID, file.timeVarID, "units", units);
    CHECK_NC_GET_ATT(ret, file.fileName, "time", "units");
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
        time += timeValue*TimeUnit::DAYS;
    } else {
        REPORT_ERROR("Unsupported time units!");
    }
    timeManager.reset(timeStep, time);
}

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::input(int fileIdx,
                                    const TimeLevelIndex<NumTimeLevel> &timeIdx,
                                    initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType, NumTimeLevel>(timeIdx, fields);
}

template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::input(int fileIdx, initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType>(fields);
}

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::input(int fileIdx,
                                    const TimeLevelIndex<NumTimeLevel> &timeIdx,
                                    int timeCounter,
                                    initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType, NumTimeLevel>(timeIdx, timeCounter, fields);
}

template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::input(int fileIdx, int timeCounter,
                                    initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType>(timeCounter, fields);
}

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::output(int fileIdx,
                                     const TimeLevelIndex<NumTimeLevel> &timeIdx,
                                     initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    // write time
    double time = timeManager->getDays();
    size_t index[1] = {0};
    int ret = nc_put_var1(file.fileID, file.timeVarID, index, &time);
    CHECK_NC_PUT_VAR(ret, file.fileName, "time");
    // write fields
    file.template output<DataType, NumTimeLevel>(timeIdx, fields);
}
    
template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::output(int fileIdx,
                                     initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    // write time
    // FIXME: Do we need to write time?
    double time = timeManager->getDays();
    size_t index[1] = {0};
    int ret = nc_put_var1(file.fileID, file.timeVarID, index, &time);
    CHECK_NC_PUT_VAR(ret, file.fileName, "time");
    // write fields
    file.template output<DataType>(fields);
}

template <class DataFileType>
void IOManager<DataFileType>::close(int fileIdx) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    CHECK_NC_CLOSE(nc_close(file.fileID), file.fileName);
    file.isActive = false;
}

}
