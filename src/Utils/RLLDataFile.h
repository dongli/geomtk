#ifndef __GEOMTK_RLLDataFile__
#define __GEOMTK_RLLDataFile__

#include "StructuredDataFile.h"
#include "RLLField.h"

namespace geomtk {

struct RLLSpaceDimensions : public StructuredSpaceDimensions {
};

class RLLDataFile
: public StructuredDataFile<RLLMesh> {
public:
    typedef RLLSpaceDimensions SpaceDimensions;

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

#endif // __GEOMTK_RLLDataFile__
