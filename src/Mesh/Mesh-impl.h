#include "Mesh.h"
#include "SphereDomain.h"

namespace geomtk {

template <class DomainType, class CoordType>
Mesh<DomainType, CoordType>::Mesh(DomainType &domain) {
    this->domain = &domain;
    set = false;
}

template <class DomainType, class CoordType>
Mesh<DomainType, CoordType>::~Mesh() {
}

template <class DomainType, class CoordType>
void Mesh<DomainType, CoordType>::init(const string &fileName) {
    if (!set) {
        REPORT_ERROR("This method should be called after the subclass version!");
    }
}

template <class DomainType, class CoordType>
void Mesh<DomainType, CoordType>::init(const string &fileNameH, const string &fileNameV) {
    if (!set) {
        REPORT_ERROR("This method should be called after the subclass version!");
    }
}

template <class DomainType, class CoordType>
int Mesh<DomainType, CoordType>::getLevelIndex(int i, int loc) const {
    REPORT_ERROR("Subclass should provide this method!");
}

}
