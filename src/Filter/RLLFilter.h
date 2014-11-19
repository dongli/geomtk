#ifndef __Geomtk_RLLFilter__
#define __Geomtk_RLLFilter__

#include "StructuredFilter.h"

namespace geomtk {

template <class MeshType>
class RLLFilter : public StructuredFilter<MeshType> {
protected:
public:
    RLLFilter(const MeshType &mesh, FilterScheme scheme) : StructuredFilter<MeshType>(mesh, scheme) {}
    virtual ~RLLFilter() {}
};

} // geomtk

#endif // __Geomtk_RLLFilter__
