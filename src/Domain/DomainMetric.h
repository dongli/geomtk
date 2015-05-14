#ifndef __GEOMTK_DomainMetric__
#define __GEOMTK_DomainMetric__

namespace geomtk {

/**
 *  This is a policy class for MLPACK.
 */
template <class DomainType>
class DomainMetric {
public:
    static const DomainType *domain;

    DomainMetric() {}

    template <typename VecType1, typename VecType2>
    static double
    Evaluate(const VecType1 &x, const VecType2 &y) {
        return domain->calcDistance(x, y);
    }

    string ToString() const { return "Implement me!"; }
}; // DomainMetric

template <class DomainType>
const DomainType* DomainMetric<DomainType>::domain = NULL;

} // geomtk

#endif // __GEOMTK_DomainMetric__
