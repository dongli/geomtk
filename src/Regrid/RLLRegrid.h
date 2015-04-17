#ifndef __Geomtk_RLLRegrid__
#define __Geomtk_RLLRegrid__

#include "Regrid.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "RLLField.h"
#include "RLLVelocityField.h"

namespace geomtk {

class RLLRegrid : public Regrid<RLLMesh, SphereCoord> {
public:
    typedef RLLStagger::GridType GridType;
    typedef RLLStagger::Location Location;
protected:
public:
    RLLRegrid();
    RLLRegrid(const RLLMesh &mesh);
    virtual ~RLLRegrid();

    template <typename T, int N>
    void run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
             const RLLField<T, N> &f, const SphereCoord &x, T &y,
             RLLMeshIndex *idx = NULL);

    void run(RegridMethod method, const TimeLevelIndex<2> &timeIdx,
             const RLLVelocityField &f, const SphereCoord &x, SphereVelocity &y,
             RLLMeshIndex *idx = NULL);
};

} // geomtk

#include "RLLRegrid-impl.h"

#endif // __Geomtk_RLLRegrid__
