#ifndef __GEOMTK_DataFile__
#define __GEOMTK_DataFile__

#include "geomtk_commons.h"
#include "Field.h"
#include "StampString.h"
#include "SystemTools.h"

namespace geomtk {

template <class MeshType>
struct FieldInfo {
    Field<MeshType> *field;
    nc_type xtype;
    int varId;
    int spaceDims;
};

enum IOType {
    INPUT, OUTPUT
};

template <class _MeshType>
class DataFile {
public:
    typedef _MeshType MeshType;
    StampString filePattern;
    string filePath;
    int fileId;
    int timeDimId, timeVarId;
    vector<FieldInfo<MeshType> > fieldInfos;
    IOType ioType;
    duration freq;
    double lastTime;
    int alarmIdx;
    bool isActive;
protected:
    MeshType *_mesh;
    TimeManager *_timeManager;
public:
    DataFile(MeshType &mesh, TimeManager &timeManager) {
        _mesh = &mesh;
        _timeManager = &timeManager;
    }
    virtual ~DataFile() {}

    MeshType&
    mesh() {
        return *_mesh;
    }

    virtual void
    addField(const string &xtype, int spaceDims,
             initializer_list<Field<MeshType>*> fields) = 0;
    
    virtual void
    removeField(initializer_list<Field<MeshType>*> fields) = 0;

    virtual void
    open(const TimeManager &timeManager) = 0;

    virtual void
    create(const TimeManager &timeManager) = 0;

    virtual string
    currentFilePath() const {
        return filePattern.run(*_timeManager);
    }

    virtual void
    inputMesh() = 0;

    virtual void
    inputHorizontalMesh() = 0;

    virtual void
    inputVerticalMesh() = 0;

    virtual void
    outputMesh() = 0;

    template <typename AttType>
    typename enable_if<!is_same<AttType, string>::value, AttType>::type
    getAttribute(const string &attName) const {
        AttType res;
        string filePath;
        nc_type attTypeInFile;
        if (_timeManager->isInited()) {
            filePath = filePattern.run(*_timeManager);
        } else {
            vector<string> filePaths = SystemTools::getFilePaths(filePattern.wildcard());
            filePath = filePaths.front();
        }
        int ncId, ret;
        ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
        CHECK_NC_OPEN(ret, filePath);
        ret = nc_inq_atttype(ncId, NC_GLOBAL, attName.c_str(), &attTypeInFile);
        CHECK_NC_INQ_ATTTYPE(ret, filePath, "global", attName);
        switch (attTypeInFile) {
            case NC_FLOAT:
                if (typeid(AttType) != typeid(float)) {
                    REPORT_ERROR("Type of attribute \"" << attName <<
                                 "\" is float, but template argument is not!");
                }
                break;
            case NC_DOUBLE:
                if (typeid(AttType) != typeid(double)) {
                    REPORT_ERROR("Type of attribute \"" << attName <<
                                 "\" is double, but template argument is not!");
                }
                break;
            case NC_INT:
                if (typeid(AttType) != typeid(int)) {
                    REPORT_ERROR("Type of attribute \"" << attName <<
                                 "\" is int, but template argument is not!");
                }
                break;
            default:
                REPORT_ERROR("Under construction!");
                break;
        }
        ret = nc_get_att(ncId, NC_GLOBAL, attName.c_str(), &res);
        CHECK_NC_GET_ATT(ret, filePath, "global", attName);
        ret = nc_close(ncId);
        CHECK_NC_CLOSE(ret, filePath);
        return res;
    }

    template <typename AttType>
    typename enable_if<is_same<AttType, string>::value, AttType>::type
    getAttribute(const string &attName) const {
        AttType res;
        string filePath = filePattern.run(*_timeManager);
        int ncId, ret;
        ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
        CHECK_NC_OPEN(ret, filePath);
        char str[100];
        memset(&str[0], 0, sizeof(str));
        ret = nc_get_att(ncId, NC_GLOBAL, attName.c_str(), str);
        CHECK_NC_GET_ATT(ret, filePath, "global", attName);
        size_t len;
        ret = nc_inq_attlen(ncId, NC_GLOBAL, attName.c_str(), &len);
        CHECK_NC_INQ_ATTLEN(ret, filePath, "global", attName);
        res.assign(str, len);
        ret = nc_close(ncId);
        CHECK_NC_CLOSE(ret, filePath);
        return res;
    }

    template <typename AttType>
    typename enable_if<!is_same<AttType, string>::value, AttType>::type
    getAttribute(const string &varName, const string &attName) const {
        AttType res;
        string filePath = filePattern.run(*_timeManager);
        int ncId, varId, ret;
        ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
        CHECK_NC_OPEN(ret, filePath);
        if (varName == "_file_") {
            varId = NC_GLOBAL;
        } else {
            ret = nc_inq_varid(ncId, varName.c_str(), &varId);
            CHECK_NC_INQ_VARID(ret, filePath, varName);
        }
        ret = nc_get_att(ncId, varId, attName.c_str(), &res);
        CHECK_NC_GET_ATT(ret, filePath, varName, attName);
        ret = nc_close(ncId);
        CHECK_NC_CLOSE(ret, filePath);
        return res;
    }

    template <typename AttType>
    typename enable_if<is_same<AttType, string>::value, AttType>::type
    getAttribute(const string &varName, const string &attName) const {
        AttType res;
        string filePath = filePattern.run(*_timeManager);
        int ncId, varId, ret;
        ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
        CHECK_NC_OPEN(ret, filePath);
        if (varName == "_file_") {
            varId = NC_GLOBAL;
        } else {
            ret = nc_inq_varid(ncId, varName.c_str(), &varId);
            CHECK_NC_INQ_VARID(ret, filePath, varName);
        }
        char str[100];
        memset(&str[0], 0, sizeof(str));
        ret = nc_get_att(ncId, varId, attName.c_str(), str);
        CHECK_NC_GET_ATT(ret, filePath, varName, attName);
        size_t len;
        ret = nc_inq_attlen(ncId, varId, attName.c_str(), &len);
        CHECK_NC_INQ_ATTLEN(ret, filePath, varName, attName);
        res.assign(str, len);
        ret = nc_close(ncId);
        CHECK_NC_CLOSE(ret, filePath);
        return res;
    }

    bool
    hasAttribute(const string &attName) const {
        string filePath;
        if (_timeManager->isInited()) {
            filePath = filePattern.run(*_timeManager);
        } else {
            vector<string> filePaths = SystemTools::getFilePaths(filePattern.wildcard());
            filePath = filePaths.front();
        }
        int ncId, ret, attId;
        ret = nc_open(filePath.c_str(), NC_NOWRITE, &ncId);
        CHECK_NC_OPEN(ret, filePath);
        ret = nc_inq_attid(ncId, NC_GLOBAL, attName.c_str(), &attId);
        nc_close(ncId);
        if (ret == NC_NOERR) {
            return true;
        } else {
            return false;
        }
    }
}; // DataFile

} // geomtk

#endif // __GEOMTK_DataFile__
