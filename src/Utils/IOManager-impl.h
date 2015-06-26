#include "IOManager.h"

namespace geomtk {
 
template <class DataFileType>
IOManager<DataFileType>::IOManager() {
    this->timeManager = NULL;
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
              const duration &freq) {
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
    file.freq = freq;
    file.alarmIdx = timeManager->addAlarm(freq);
    file.isActive = false;
    file.lastTime = -1;
    files.push_back(file);
    REPORT_NOTICE("Add output file with pattern " << filePattern << ".");
    return files.size()-1;
} // addOutputFile

template <class DataFileType>
int IOManager<DataFileType>::
addOutputFile(typename DataFileType::MeshType &mesh, const string &filePattern,
              const duration &freq) {
    StampString tmp(filePattern);
    return addOutputFile(mesh, tmp, freq);
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
    // Define temporal dimension.
    int timeStep = timeManager->numStep();
    ret = nc_put_att(file.fileId, NC_GLOBAL, "time_step", NC_INT, 1, &timeStep);
    CHECK_NC_PUT_ATT(ret, file.filePath, "NC_GLOBAL", "time_step");
    double stepSize = timeManager->stepSizeInSeconds();
    ret = nc_put_att(file.fileId, NC_GLOBAL, "time_step_size_in_seconds", NC_DOUBLE, 1, &stepSize);
    CHECK_NC_PUT_ATT(ret, file.filePath, "NC_GLOBAL", "time_step_size_in_seconds");
    ret = nc_def_dim(file.fileId, "time", NC_UNLIMITED, &file.timeDimId);
    CHECK_NC_DEF_DIM(ret, file.filePath, "time");
    ret = nc_def_var(file.fileId, "time", NC_DOUBLE, 1, &file.timeDimId, &file.timeVarId);
    CHECK_NC_DEF_VAR(ret, file.filePath, "time")
    string units = "days since "+ptime_to_string(timeManager->startTime());
    ret = nc_put_att(file.fileId, file.timeVarId, "units", NC_CHAR,
                     units.length(), units.c_str());
    CHECK_NC_PUT_ATT(ret, file.filePath, "time", "units");
    // Let concrete data file class create the rest data file.
    file.create(*timeManager);
    file.outputDomain();
    file.outputMesh();
} // create

template <class DataFileType>
ptime IOManager<DataFileType>::
getTime(uword fileIdx) const {
    ptime res;
    const DataFileType &file = files[fileIdx];
    int ret; double timeValue;
    char unitsInFile[100], unitsInSeconds[100];
    ret = nc_get_var(file.fileId, file.timeVarId, &timeValue);
    CHECK_NC_GET_VAR(ret, file.filePath, "time");
    ret = nc_get_att(file.fileId, file.timeVarId, "units", unitsInFile);
    CHECK_NC_GET_ATT(ret, file.filePath, "time", "units");
    regex reDate("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d) (\\d\\d)*:(\\d\\d)*:(\\d\\.\\d*)*");
    match_results<std::string::const_iterator> what;
    if (regex_search(string(unitsInFile), what, reDate)) {
        sprintf(unitsInSeconds, "seconds since %s", what[0].str().c_str());
        res = time_from_string(what[0]);
    }
    ut_set_error_message_handler(ut_ignore);
    ut_system *utSystem = ut_read_xml(NULL);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to get units database!");
    }
    ut_unit *utTimeUnit1 = ut_parse(utSystem, unitsInFile, UT_ASCII);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to parse time unit \"" << unitsInFile << "\"!");
    }
    ut_unit *utTimeUnit2 = ut_parse(utSystem, unitsInSeconds, UT_ASCII);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to parse time unit \"" << unitsInSeconds << "\"!");
    }
    cv_converter *cvConverter = ut_get_converter(utTimeUnit1, utTimeUnit2);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to get units converter!");
    }
    res += seconds(cv_convert_double(cvConverter, timeValue));
    return res;
} // getTime

template <class DataFileType>
ptime IOManager<DataFileType>::
getTime(const string &filePath) {
    ptime res;
    int ret, fileId, timeVarId;
    double timeValue;
    char unitsInFile[100], unitsInSeconds[100];
    memset(&unitsInFile[0], 0, sizeof(unitsInFile));
    ret = nc_open(filePath.c_str(), NC_NOWRITE, &fileId);
    CHECK_NC_OPEN(ret, filePath);
    ret = nc_inq_varid(fileId, "time", &timeVarId);
    CHECK_NC_INQ_VARID(ret, filePath, "time");
    ret = nc_get_var(fileId, timeVarId, &timeValue);
    CHECK_NC_GET_VAR(ret, filePath, "time");
    ret = nc_get_att_text(fileId, timeVarId, "units", unitsInFile);
    CHECK_NC_GET_ATT(ret, filePath, "time", "units");
    ret = nc_close(fileId);
    CHECK_NC_CLOSE(ret, filePath);
    regex reDate("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d) (\\d\\d)*:(\\d\\d)*:(\\d\\.\\d*)*");
    match_results<std::string::const_iterator> what;
    if (regex_search(string(unitsInFile), what, reDate)) {
        sprintf(unitsInSeconds, "seconds since %s", what[0].str().c_str());
        res = time_from_string(what[0]);
    }
    ut_set_error_message_handler(ut_ignore);
    ut_system *utSystem = ut_read_xml(NULL);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to get units database!");
    }
    ut_unit *utTimeUnit1 = ut_parse(utSystem, unitsInFile, UT_ASCII);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to parse time unit \"" << unitsInFile << "\"!");
    }
    ut_unit *utTimeUnit2 = ut_parse(utSystem, unitsInSeconds, UT_ASCII);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to parse time unit \"" << unitsInSeconds << "\"!");
    }
    cv_converter *cvConverter = ut_get_converter(utTimeUnit1, utTimeUnit2);
    if (ut_get_status() != UT_SUCCESS) {
        REPORT_ERROR("udunits: Failed to get units converter!");
    }
    res += seconds(cv_convert_double(cvConverter, timeValue));
    return res;
} // getTime

template <class DataFileType>
void IOManager<DataFileType>::
updateTime(uword fileIdx, TimeManager &timeManager) {
    DataFileType &file = files[fileIdx];
    int timeStep, ret;
    ret = nc_get_att(file.fileId, NC_GLOBAL, "time_step", &timeStep);
    CHECK_NC_GET_ATT(ret, file.filePath, "NC_GLOBAL", "time_step");
    ptime time = getTime(fileIdx);
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
