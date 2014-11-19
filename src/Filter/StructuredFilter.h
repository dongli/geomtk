#ifndef __Geomtk_StructuredFilter__
#define __Geomtk_StructuredFilter__

#include "Filter.h"

namespace geomtk {

template <class MeshType>
class StructuredFilter : public Filter<MeshType> {
protected:
    typedef StructuredStagger::GridType GridType;
    typedef StructuredStagger::Location Location;
public:
    StructuredFilter(const MeshType &mesh, FilterScheme scheme) : Filter<MeshType>(mesh, scheme) {}
    virtual ~StructuredFilter() {}

    template <class FieldType, int N>
    void run(const TimeLevelIndex<N> &timeIdx, FieldType &field);
private:
    template <class FieldType, int N>
    void runNinePointSmoothing(const TimeLevelIndex<N> &timeIdx, FieldType &field);
};

} // geomtk

#include "StructuredFilter-impl.h"

#endif // __Geomtk_StructuredFilter__
