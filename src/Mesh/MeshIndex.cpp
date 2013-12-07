#include "MeshIndex.h"

namespace geomtk {

MeshIndex::MeshIndex(Mesh &mesh) {
    this->mesh = &mesh;
}

MeshIndex::~MeshIndex() {
}

}
