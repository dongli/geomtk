#ifndef __Geomtk_RLLDataFile__
#define __Geomtk_RLLDataFile__

#include "StructuredDataFile.h"
#include "RLLField.h"

namespace geomtk {

struct RLLSpaceDimensions : public StructuredSpaceDimensions {
};

class RLLDataFile : public StructuredDataFile<RLLMesh> {
public:
    RLLDataFile(MeshType &mesh) : StructuredDataFile<RLLMesh>(mesh) {}
    virtual ~RLLDataFile() {}

    virtual void inputGrids();

    virtual void inputHorizontalGrids();

    virtual void inputVerticalGrids();

    virtual void outputGrids();
};

} // geomtk

#endif // __Geomtk_RLLDataFile__
