#ifndef __GEOMTK_RLLField__
#define __GEOMTK_RLLField__

#include "StructuredField.h"
#include "RLLMesh.h"

namespace geomtk {

template <typename DataType, int NumTimeLevel = 1>
class RLLField : public StructuredField<RLLMesh, DataType, NumTimeLevel> {
public:
	typedef RLLMesh MeshType;

    RLLField() {}
    virtual ~RLLField() {}
}; // RLLField

} // geomtk

#endif // __GEOMTK_RLLField__

