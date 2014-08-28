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
private:
    TimeManager *timeManager;
    vector<DataFileType> files;
public:
    typedef typename DataFileType::MeshType MeshType;

    IOManager();
    virtual ~IOManager();

    void init(TimeManager &timeManager);

    int registerInputFile(MeshType &mesh, const string &fileName);

    int registerOutputFile(MeshType &mesh,
                           StampString &filePattern,
                           TimeStepUnit freqUnit, double freq);

    int registerOutputFile(MeshType &mesh,
                           const string &filePattern,
                           TimeStepUnit freqUnit, double freq);

    void removeFile(int fileIdx);

    DataFileType& file(int fileIdx);

    void open(int fileIdx);

    void create(int fileIdx);

    void updateTime(int fileIdx, TimeManager &timeManager);

    template <typename DataType, int NumTimeLevel>
    void input(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
               initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void input(int fileIdx, initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void input(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
               int timeCounter, initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void input(int fileIdx, int timeCounter,
               initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void output(int fileIdx, const TimeLevelIndex<NumTimeLevel> &timeIdx,
               initializer_list<Field<MeshType>*> fields);
    
    template <typename DataType>
    void output(int fileIdx, initializer_list<Field<MeshType>*> fields);

    void close(int fileIdx);

    /**
     *  Check if file is active for input or output.
     *
     *  @param fileIdx the data file index.
     *
     *  @return The boolean result.
     */
    bool isFileActive(int fileIdx);
};

} // geomtk

#include "IOManager-impl.h"

#endif // __Geomtk_IOManager__
