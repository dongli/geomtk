#ifndef __GEOMTK_StructuredFilter__
#define __GEOMTK_StructuredFilter__

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
}; // StructuredFilter

} // geomtk

#include "StructuredFilter-impl.h"

#endif // __GEOMTK_StructuredFilter__
