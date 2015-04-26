#ifndef __GEOMTK_IOManager__
#define __GEOMTK_IOManager__

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
    removeFile(uword fileIdx);

    DataFileType&
    file(uword fileIdx);

    void
    addField(uword fileIdx, const string &xtype, int spaceDims,
             initializer_list<Field<MeshType>*> fields);

    void
    open(uword fileIdx);

    void
    create(uword fileIdx);

    Time
    getTime(uword fileIdx) const;

    Time
    getTime(const string &filePath) const;

    void
    updateTime(uword fileIdx, TimeManager &timeManager);

    template <typename DataType, int NumTimeLevel>
    void
    input(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(uword fileIdx, initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    input(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
          int timeCounter, initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(uword fileIdx, int timeCounter,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    output(uword fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
           initializer_list<Field<MeshType>*> fields);
    
    template <typename DataType>
    void
    output(uword fileIdx, initializer_list<Field<MeshType>*> fields);

    void
    close(uword fileIdx);

    /**
     *  Check if file is active for input or output.
     *
     *  @param fileIdx the data file index.
     *
     *  @return The boolean result.
     */
    bool
    isFileActive(uword fileIdx);
}; // IOManager

} // geomtk

#include "IOManager-impl.h"

#endif // __GEOMTK_IOManager__
