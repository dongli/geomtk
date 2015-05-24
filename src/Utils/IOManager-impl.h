#include "IOManager.h"

namespace geomtk {
 
template <class DataFileType>
IOManager<DataFileType>::IOManager() {
}

template <class DataFileType>
IOManager<DataFileType>::~IOManager() {
}

template <class DataFileType>
void IOManager<DataFileType>::
init(TimeManager &timeManager) {
    this->timeManager = &timeManager;
} // init

template <class DataFileType>
int IOManager<DataFileType>::
addInputFile(MeshType &mesh, const string &filePattern) {
    for (uword i = 0; i < files.size(); ++i) {
        if (files[i].ioType == INPUT && files[i].filePattern == filePattern) {
            REPORT_ERROR("File with file pattern \"" << filePattern <<
                         "\" has already been added for input!");
        }
    }
    DataFileType file(mesh, *timeManager);
    file.filePattern = filePattern;
    file.ioType = INPUT;
    file.isActive = true;
    files.push_back(file);
    REPORT_NOTICE("Register input file with pattern " << filePattern << ".");
    return files.size()-1;
} // addInputFile

template <class DataFileType>
int IOManager<DataFileType>::
addOutputFile(typename DataFileType::MeshType &mesh, StampString &filePattern,
              TimeStepUnit freqUnit, double freq) {
    for (uword i = 0; i < files.size(); ++i) {
        if (files[i].ioType == OUTPUT && files[i].filePattern == filePattern) {
            REPORT_ERROR("File with pattern \"" << filePattern <<
                         "\" has already been added for output!");
        }
    }
    // add an alarm for output
    DataFileType file(mesh, *timeManager);
    file.filePattern = filePattern;
    file.ioType = OUTPUT;
    file.freqUnit = freqUnit;
    file.freq = freq;
    file.alarmIdx = timeManager->addAlarm(freqUnit, freq);
    file.isActive = false;
    file.lastTime = -1;
    files.push_back(file);
    REPORT_NOTICE("Add output file with pattern " << filePattern << ".");
    return files.size()-1;
} // addOutputFile

template <class DataFileType>
int IOManager<DataFileType>::
addOutputFile(typename DataFileType::MeshType &mesh, const string &filePattern,
              TimeStepUnit freqUnit, double freq) {
    StampString tmp(filePattern);
    return addOutputFile(mesh, tmp, freqUnit, freq);
} // addOutputFile

template <class DataFileType>
void IOManager<DataFileType>::
removeFile(uword fileIdx) {
    if (fileIdx < 0 || fileIdx >= files.size()) {
        REPORT_ERROR("File index is out of range!");
    }
    files.erase(files.begin()+fileIdx);
} // removeFile

template <class DataFileType>
DataFileType& IOManager<DataFileType>::
file(uword fileIdx) {
    if (fileIdx < 0 || fileIdx >= files.size()) {
        REPORT_ERROR("File index is out of range!");
    }
    return files[fileIdx];
} // file

template <class DataFileType>
void IOManager<DataFileType>::
addField(uword fileIdx, const string &xtype, int spaceDims,
         initializer_list<Field<MeshType>*> fields) {
    file(fileIdx).addField(xtype, spaceDims, fields);
} // addField

template <class DataFileType>
bool IOManager<DataFileType>::
isFileActive(uword fileIdx) {
    DataFileType &file = files[fileIdx];
    file.isActive = timeManager->checkAlarm(file.alarmIdx);
    return file.isActive;
} // isFileActive

template <class DataFileType>
void IOManager<DataFileType>::
open(uword fileIdx) {
    DataFileType &file = files[fileIdx];
    file.filePath = file.filePattern.run(*timeManager);
    int ret;
    ret = nc_open(file.filePath.c_str(), NC_NOWRITE, &file.fileId);
    CHECK_NC_OPEN(ret, file.filePath);
    ret = nc_inq_varid(file.fileId, "time", &file.timeVarId);
    if (ret != NC_NOERR) {
        file.timeVarId = -999;
    }
    // let concrete data file class open the rest data file
    file.open(*timeManager);
} // open
    
template <class DataFileType>
void IOManager<DataFileType>::
create(uword fileIdx) {
    DataFileType &file = files[fileIdx];
    if (!isFileActive(fileIdx)) return;
    file.filePath = file.filePattern.run(*timeManager);
    int ret = nc_create(file.filePath.c_str(), NC_CLOBBER, &file.fileId);
    CHECK_NC_CREATE(ret, file.filePath);
    // define temporal dimension
    int timeStep = timeManager->numStep();
    ret = nc_put_att(file.fileId, NC_GLOBAL, "time_step", NC_INT, 1, &timeStep);
    CHECK_NC_PUT_ATT(ret, file.filePath, "NC_GLOBAL", "time_step");
    ret = nc_def_dim(file.fileId, "time", NC_UNLIMITED, &file.timeDimId);
    CHECK_NC_DEF_DIM(ret, file.filePath, "time");
    ret = nc_def_var(file.fileId, "time", NC_DOUBLE, 1, &file.timeDimId, &file.timeVarId);
    CHECK_NC_DEF_VAR(ret, file.filePath, "time")
    string units = "days since "+timeManager->startTime().s();
    ret = nc_put_att(file.fileId, file.timeVarId, "units", NC_CHAR,
                     units.length(), units.c_str());
    CHECK_NC_PUT_ATT(ret, file.filePath, "time", "units");
    // let concrete data file class create the rest data file
    file.create(*timeManager);
    file.outputMesh();
} // create

template <class DataFileType>
Time IOManager<DataFileType>::
getTime(uword fileIdx) const {
    Time time;
    const DataFileType &file = files[fileIdx];
    int ret; double timeValue; char units[100];
    ret = nc_get_var(file.fileId, file.timeVarId, &timeValue);
    CHECK_NC_GET_VAR(ret, file.filePath, "time");
    ret = nc_get_att(file.fileId, file.timeVarId, "units", units);
    CHECK_NC_GET_ATT(ret, file.filePath, "time", "units");
    regex reDays("^days");
    regex reDate("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d) (\\d\\d)*:(\\d\\d)*:(\\d\\.\\d*)*");
    match_results<std::string::const_iterator> what; string tmp = units;
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
    return time;
} // getTime

template <class DataFileType>
Time IOManager<DataFileType>::
getTime(const string &filePath) const {
    Time time;
    int ret, fileId, timeVarId; double timeValue; char units[100];
    ret = nc_open(filePath.c_str(), NC_NOWRITE, &fileId);
    CHECK_NC_OPEN(ret, filePath);
    ret = nc_inq_varid(fileId, "time", &timeVarId);
    CHECK_NC_INQ_VARID(ret, filePath, "time");
    ret = nc_get_var(fileId, timeVarId, &timeValue);
    CHECK_NC_GET_VAR(ret, filePath, "time");
    ret = nc_get_att(fileId, timeVarId, "units", units);
    CHECK_NC_GET_ATT(ret, filePath, "time", "units");
    ret = nc_close(fileId);
    CHECK_NC_CLOSE(ret, filePath);
    regex reDays("^days");
    regex reDate("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d) (\\d\\d)*:(\\d\\d)*:(\\d\\.\\d*)*");
    match_results<std::string::const_iterator> what; string tmp = units;
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
    return time;
} // getTime

template <class DataFileType>
void IOManager<DataFileType>::
updateTime(uword fileIdx, TimeManager &timeManager) {
    DataFileType &file = files[fileIdx];
    int timeStep, ret;
    ret = nc_get_att(file.fileId, NC_GLOBAL, "time_step", &timeStep);
    CHECK_NC_GET_ATT(ret, file.filePath, "NC_GLOBAL", "time_step");
    Time time = getTime(fileIdx);
    timeManager.reset(timeStep, time);
} // updateTime

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::
input(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
      initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType, NumTimeLevel>(timeIdx, fields);
} // input

template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::
input(uword fileIdx, initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType>(fields);
} // input

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::
input(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
      int timeCounter, initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType, NumTimeLevel>(timeIdx, timeCounter, fields);
} // input

template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::
input(uword fileIdx, int timeCounter,
      initializer_list<Field<MeshType>*> fields) {
    DataFileType &file = files[fileIdx];
    file.template input<DataType>(timeCounter, fields);
} // input

template <class DataFileType>
template <typename DataType, int NumTimeLevel>
void IOManager<DataFileType>::
output(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
       initializer_list<const Field<MeshType>*> fields) {
    int ret, flag = 0;
    DataFileType &file = files[fileIdx];
    // If the file is not created yet, then create it.
    ret = nc_inq_format(file.fileId, &flag);
    if (ret != NC_NOERR) {
        flag = -999;
        create(fileIdx);
    }
    if (!file.isActive) return;
    // Write time
    double time = timeManager->days();
    size_t index[1] = {0};
    ret = nc_put_var1(file.fileId, file.timeVarId, index, &time);
    CHECK_NC_PUT_VAR(ret, file.filePath, "time");
    // Write fields
    file.template output<DataType, NumTimeLevel>(timeIdx, fields);
    // If the file is just created, then close it.
    if (flag == -999) {
        close(fileIdx);
    }
} // output
    
template <class DataFileType>
template <typename DataType>
void IOManager<DataFileType>::
output(uword fileIdx, initializer_list<const Field<MeshType>*> fields) {
    int ret, flag = 0;
    DataFileType &file = files[fileIdx];
    // If the file is not created yet, then create it.
    ret = nc_inq_format(file.fileId, &flag);
    if (ret != NC_NOERR) {
        flag = -999;
        create(fileIdx);
    }
    if (!file.isActive) return;
    // Write time
    // FIXME: Do we need to write time?
    double time = timeManager->days();
    size_t index[1] = {0};
    ret = nc_put_var1(file.fileId, file.timeVarId, index, &time);
    CHECK_NC_PUT_VAR(ret, file.filePath, "time");
    // Write fields
    file.template output<DataType>(fields);
    // If the file is just created, then close it.
    if (flag == -999) {
        close(fileIdx);
    }
} // output

template <class DataFileType>
void IOManager<DataFileType>::
close(uword fileIdx) {
    DataFileType &file = files[fileIdx];
    if (!file.isActive) return;
    CHECK_NC_CLOSE(nc_close(file.fileId), file.filePath);
    file.isActive = false;
} // close

} // geomtk
