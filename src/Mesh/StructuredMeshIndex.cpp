#include "StructuredMeshIndex.h"

StructuredMeshIndex::StructuredMeshIndex(Mesh &mesh) : MeshIndex(mesh) {
    indices[CENTER].resize(mesh.getDomain().getNumDim());
    indices[EDGE].resize(mesh.getDomain().getNumDim());
    indices[VERTEX].resize(mesh.getDomain().getNumDim());
    reset();
}

StructuredMeshIndex::~StructuredMeshIndex() {
}

void StructuredMeshIndex::reset() {
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        indices[CENTER][m] = UNDEFINED_MESH_INDEX;
        indices[EDGE][m] = UNDEFINED_MESH_INDEX;
        indices[VERTEX][m] = UNDEFINED_MESH_INDEX;
    }
}

int StructuredMeshIndex::operator()(int dim, StaggerType staggerType) const {
    return indices[staggerType][dim];
}

int& StructuredMeshIndex::operator()(int dim, StaggerType staggerType) {
    return indices[staggerType][dim];
}

void StructuredMeshIndex::locate(SpaceCoord &x) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
        // sanity check
        if (mesh.getDomain().getAxisStartBndType(m) == PERIODIC) {
            if (x(m) < mesh.fullCoords[m](1) ||
                x(m) > mesh.fullCoords[m](mesh.fullCoords[m].size()-1)) {
                REPORT_ERROR("Coordinate x is out of range on dimension " << m << "!");
            }
        } else if (mesh.getDomain().getAxisStartBndType(m) == POLE) {
            if (x(m) < mesh.fullCoords[m](0) ||
                x(m) > mesh.fullCoords[m](mesh.fullCoords[m].size()-1)) {
                REPORT_ERROR("Coordinate x is out of range on dimension " << m << "!");
            }
        } else if (mesh.getDomain().getAxisStartBndType(m) == RIGID) {
            if (x(m) < mesh.fullCoords[m](0)) {
                REPORT_ERROR("Coordinate x is out of range on dimension " << m << "!");
            }
        } else if (mesh.getDomain().getAxisEndBndType(m) == RIGID) {
            if (x(m) > mesh.fullCoords[m](mesh.fullCoords[m].size()-1)) {
                REPORT_ERROR("Coordinate x is out of range on dimension " << m << "!");
            }
        }
        if (indices[CENTER][m] == UNDEFINED_MESH_INDEX) {
            for (int i = 0; i < mesh.fullCoords[m].size()-1; ++i) {
                if (x(m) >= mesh.fullCoords[m](i) && x(m) <= mesh.fullCoords[m](i+1)) {
                    // cout << setw(5) << m << setw(5) << i;
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.fullCoords[m](i);
                    // cout << setw(10) << mesh.fullCoords[m](i+1) << endl;
                    indices[CENTER][m] = i;
                    break;
                }
            }
            if (mesh.getDomain().getAxisStartBndType(m) == POLE) {
                if (x(m) < mesh.halfCoords[m](0)) {
                    indices[EDGE][m] = -1;
                    continue;
                } else if (x(m) > mesh.halfCoords[m](mesh.halfCoords[m].size()-1)) {
                    indices[EDGE][m] = mesh.halfCoords[m].size()-1;
                    continue;
                }
            }
            for (int i = 0; i < mesh.halfCoords[m].size()-1; ++i) {
                if (x(m) >= mesh.halfCoords[m](i) && x(m) <= mesh.halfCoords[m](i+1)) {
                    // cout << setw(5) << m << setw(5) << i;
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.halfCoords[m](i);
                    // cout << setw(10) << mesh.halfCoords[m](i+1) << endl;
                    indices[EDGE][m] = i;
                    break;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}
