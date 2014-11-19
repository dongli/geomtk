#ifndef __Geomtk_Filter__
#define __Geomtk_Filter__

#include "geomtk_commons.h"

namespace geomtk {

enum FilterScheme {
    NINE_POINT_SMOOTHING
};

template <class MeshType>
class Filter {
protected:
    const MeshType *mesh;
    FilterScheme scheme;
public:
    Filter(const MeshType &mesh, FilterScheme scheme) {
        this->mesh = &mesh;
        this->scheme = scheme;
    }
    virtual ~Filter() {}
};

} // geomtk

#endif // __Geomtk_Filter__
