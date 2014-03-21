#ifndef __Geomtk_NumericRLLField__
#define __Geomtk_NumericRLLField__

#include "NumericStructuredField.h"
#include "RLLMesh.h"

namespace geomtk {

template <typename T, int N>
class NumericRLLField : public NumericStructuredField<T, N> {
public:
    NumericRLLField();
    virtual ~NumericRLLField();
};

// -----------------------------------------------------------------------------

template <typename T, int N>
NumericRLLField<T, N>::NumericRLLField() : NumericStructuredField<T, N>() {
}

template <typename T, int N>
NumericRLLField<T, N>::~NumericRLLField() {
}

}

#endif

