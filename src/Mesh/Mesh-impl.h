#include "Mesh.h"
#include "SphereDomain.h"

namespace geomtk {

template <class DomainType, class CoordType>
Mesh<DomainType, CoordType>::Mesh(DomainType &domain) {
    this->_domain = &domain;
    set = false;
}

template <class DomainType, class CoordType>
Mesh<DomainType, CoordType>::~Mesh() {
}

} // geomtk
