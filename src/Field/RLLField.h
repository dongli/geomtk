#ifndef __Geomtk_RLLField__
#define __Geomtk_RLLField__

#include "StructuredField.h"
#include "RLLMesh.h"

namespace geomtk {

template <typename T>
class RLLField : public StructuredField<T> {
public:
    RLLField(const Mesh &mesh, bool hasHalfLevel = false);
    RLLField(const string &name, const string &units,
             const string &longName, const Mesh &mesh,
             bool hasHalfLevel = false);
    virtual ~RLLField();
};

// -----------------------------------------------------------------------------

template <typename T>
RLLField<T>::RLLField(const Mesh &mesh, bool hasHalfLevel)
: StructuredField<T>(mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply with RLLMesh!");
    }
}

template <typename T>
RLLField<T>::RLLField(const string &name, const string &units,
                               const string &longName, const Mesh &mesh,
                               bool hasHalfLevel)
: StructuredField<T>(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const RLLMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should comply with RLLMesh!");
    }
}

template <typename T>
RLLField<T>::~RLLField() {
}

}

#endif

