#ifndef __GEOMTK_Diagnostics__
#define __GEOMTK_Diagnostics__

#include "geomtk_commons.h"

namespace geomtk {

template <class MeshType, template <typename, int> class FieldType,
          class IOManagerType>
class Diagnostics {
    static MeshType *_mesh;
    static IOManagerType *_io;
    static map<string, any> _metrics;
    static int outputIdx;
public:
    static void
    init(MeshType &mesh, IOManagerType &io);

    template <typename MetricDataType>
    static void
    addMetric(const string &name, const string &units, const string &longName);

    template <typename MetricDataType>
    static void
    resetMetric(const string &name);

    template <typename MetricDataType>
    static MetricDataType&
    metric(const string &name);

    static void
    output();
}; // Diagnostics

} // geomtk

#include "Diagnostics-impl.h"

#endif // __GEOMTK_Diagnostics__
