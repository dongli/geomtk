#ifndef __GEOMTK_Filter__
#define __GEOMTK_Filter__

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
}; // Filter

} // geomtk

#endif // __GEOMTK_Filter__
