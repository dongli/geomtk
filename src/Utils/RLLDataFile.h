#ifndef __Geomtk_RLLDataFile__
#define __Geomtk_RLLDataFile__

#include "StructuredDataFile.h"
#include "RLLField.h"

namespace geomtk {

struct RLLSpaceDimensions : public StructuredSpaceDimensions {
};

class RLLDataFile
: public StructuredDataFile<RLLMesh> {
public:
    RLLDataFile(MeshType &mesh, TimeManager &timeManager)
        : StructuredDataFile<RLLMesh>(mesh, timeManager) {}
    virtual ~RLLDataFile() {}

    virtual void
    inputHorizontalMesh();

    virtual void
    inputVerticalMesh();

    virtual void
    outputMesh();
}; // RLLDataFile

} // geomtk

#endif // __Geomtk_RLLDataFile__
