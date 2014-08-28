#ifndef __Geomtk_RLLField__
#define __Geomtk_RLLField__

#include "StructuredField.h"
#include "RLLMesh.h"

namespace geomtk {

template <typename DataType, int NumTimeLevel>
class RLLField : public StructuredField<RLLMesh, DataType, NumTimeLevel> {
public:
	typedef RLLMesh MeshType;

    RLLField() {}
    virtual ~RLLField() {}
};

} // geomtk

#endif // __Geomtk_RLLField__

