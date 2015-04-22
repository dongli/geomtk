#ifndef __Geomtk_IOManager__
#define __Geomtk_IOManager__

#include "CartesianDataFile.h"
#include "RLLDataFile.h"
#include "TimeManager.h"

namespace geomtk {

/**
 *  This class manages the I/O for fields.
 */
template <class DataFileType>
class IOManager {
public:
    typedef typename DataFileType::MeshType MeshType;
private:
    TimeManager *timeManager;
    vector<DataFileType> files;
public:
    IOManager();
    virtual ~IOManager();

    void
    init(TimeManager &timeManager);

    int
    addInputFile(MeshType &mesh, const string &filePattern);

    int
    addOutputFile(MeshType &mesh, StampString &filePattern,
                  TimeStepUnit freqUnit, double freq);

    int
    addOutputFile(MeshType &mesh, const string &filePattern,
                  TimeStepUnit freqUnit, double freq);

    void
    removeFile(int fileIdx);

    DataFileType&
    file(int fileIdx);

    void
    addField(int fileIdx, const string &xtype, int spaceDims,
             initializer_list<Field<MeshType>*> fields);

    void
    open(int fileIdx);

    void
    create(int fileIdx);

    Time
    getTime(int fileIdx) const;

    Time
    getTime(const string &filePath) const;

    void
    updateTime(int fileIdx, TimeManager &timeManager);

    template <typename DataType, int NumTimeLevel>
    void
    input(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(int fileIdx, initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    input(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
          int timeCounter, initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(int fileIdx, int timeCounter,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    output(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
           initializer_list<Field<MeshType>*> fields);
    
    template <typename DataType>
    void
    output(int fileIdx, initializer_list<Field<MeshType>*> fields);

    void
    close(int fileIdx);

    /**
     *  Check if file is active for input or output.
     *
     *  @param fileIdx the data file index.
     *
     *  @return The boolean result.
     */
    bool
    isFileActive(int fileIdx);
}; // IOManager

} // geomtk

#include "IOManager-impl.h"

#endif // __Geomtk_IOManager__
