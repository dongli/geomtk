#ifndef __Geomtk_RLLField__
#define __Geomtk_RLLField__

#include "StructuredField.h"
#include "RLLMesh.h"

namespace geomtk {

template <typename T, int N = 2>
class RLLField : public StructuredField<T, N> {
public:
    RLLField();
    virtual ~RLLField();
};

// -----------------------------------------------------------------------------

template <typename T, int N>
RLLField<T, N>::RLLField() : StructuredField<T, N>() {
}

template <typename T, int N>
RLLField<T, N>::~RLLField() {
}

}

#endif

