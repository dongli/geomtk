#ifndef __GEOMTK_StructuredDataFile__
#define __GEOMTK_StructuredDataFile__

#include "DataFile.h"
#include "StructuredField.h"

namespace geomtk {

struct StructuredSpaceDimensions {
    static const int FULL_DIMENSION = 0;
    static const int XY_DIMENSION = 1;
    static const int YZ_DIMENSION = 2;
    static const int XZ_DIMENSION = 3;
};

template <class MeshType>
class StructuredDataFile : public DataFile<MeshType> {
public:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
    typedef StructuredSpaceDimensions SpaceDimensions;
protected:
    vector<int> fullDimIDs, fullVarIDs;
    vector<int> halfDimIDs, halfVarIDs;
    // for input only
    bool bnds2D;
    int bndsDimID;
public:
    StructuredDataFile(MeshType &mesh, TimeManager &timeManager);
    virtual ~StructuredDataFile() {}

    virtual void
    addField(const string &xtype, int spaceDims,
             initializer_list<Field<MeshType>*> fields);
    
    virtual void
    removeField(initializer_list<Field<MeshType>*> fields);

    virtual void
    open(const TimeManager &timeManager);

    virtual void
    create(const TimeManager &timeManager);

    virtual void
    inputMesh();

    virtual void
    inputHorizontalMesh();

    virtual void
    inputVerticalMesh();

    virtual void
    outputMesh();

    template <typename DataType, int NumTimeLevel>
    void
    input(const TimeLevelIndex<NumTimeLevel> &timeIdx,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    input(const TimeLevelIndex<NumTimeLevel> &timeIdx, int timeCounter,
          initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    input(int timeCounter, initializer_list<Field<MeshType>*> fields);

    template <typename DataType, int NumTimeLevel>
    void
    output(const TimeLevelIndex<NumTimeLevel> &timeIdx,
           initializer_list<Field<MeshType>*> fields);

    template <typename DataType>
    void
    output(initializer_list<Field<MeshType>*> fields);
}; // StructuredDataFile

} // geomtk

#include "StructuredDataFile-impl.h"

#endif // __GEOMTK_StructuredDataFile__
