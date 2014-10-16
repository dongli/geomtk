#ifndef __Geomtk_DataFile__
#define __Geomtk_DataFile__

#include "geomtk_commons.h"
#include "Field.h"
#include "StampString.h"

namespace geomtk {

template <class MeshType>
struct FieldInfo {
    Field<MeshType> *field;
    nc_type xtype;
    int varID;
    int spaceDims;
};

enum IOType {
    INPUT, OUTPUT
};

template <class MeshType_>
class DataFile {
public:
    typedef MeshType_ MeshType;

    StampString filePattern;
    string fileName;
    int fileID;
    int timeDimID, timeVarID;
    vector<FieldInfo<MeshType> > fieldInfos;
    IOType ioType;
    TimeStepUnit freqUnit;
    double freq;
    double lastTime;
    int alarmIdx;
    bool isActive;
protected:
    MeshType *_mesh;
public:
    DataFile(MeshType &mesh) { _mesh = &mesh; }
    virtual ~DataFile() {}

    MeshType& mesh() { return *_mesh; }

    virtual void registerField(const string &xtype, int spaceDims,
                               initializer_list<Field<MeshType>*> fields) = 0;
    
    virtual void removeField(initializer_list<Field<MeshType>*> fields) = 0;

    virtual void open(const TimeManager &timeManager) = 0;

    virtual void create(const TimeManager &timeManager) = 0;

    virtual void inputGrids() = 0;

    virtual void inputHorizontalGrids() = 0;

    virtual void inputVerticalGrids() = 0;

    virtual void outputGrids() = 0;
};

}

#endif
