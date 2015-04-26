#ifndef __GEOMTK_RLLFilter__
#define __GEOMTK_RLLFilter__

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

#endif // __GEOMTK_RLLFilter__
