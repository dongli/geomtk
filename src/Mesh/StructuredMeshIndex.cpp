#include "StructuredMeshIndex.h"

StructuredMeshIndex::StructuredMeshIndex(Mesh &mesh) : MeshIndex(mesh) {
    // NOTE: Index is 3D no matter the dimension size of domain.
    indices = new int*[3];
    for (int i = 0; i < 3; ++i) {
        indices[i] = new int[3];
    }
    reset();
}

StructuredMeshIndex::~StructuredMeshIndex() {
    for (int i = 0; i < 3; ++i) {
        delete [] indices[i];
    }
    delete [] indices;
}

void StructuredMeshIndex::reset() {
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        indices[m][CENTER] = UNDEFINED_MESH_INDEX;
        indices[m][EDGE] = UNDEFINED_MESH_INDEX;
        indices[m][VERTEX] = UNDEFINED_MESH_INDEX;
    }
}

int StructuredMeshIndex::operator()(int dim, StaggerType staggerType) const {
    return indices[dim][staggerType];
}

int& StructuredMeshIndex::operator()(int dim, StaggerType staggerType) {
    return indices[dim][staggerType];
}

void StructuredMeshIndex::locate(const SpaceCoord &x) {
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
        if (indices[m][CENTER] == UNDEFINED_MESH_INDEX) {
            for (int i = 0; i < mesh.fullCoords[m].size()-1; ++i) {
                if (x(m) >= mesh.fullCoords[m](i) && x(m) <= mesh.fullCoords[m](i+1)) {
                    // cout << setw(5) << m << setw(5) << i;
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.fullCoords[m](i);
                    // cout << setw(10) << mesh.fullCoords[m](i+1) << endl;
                    indices[m][CENTER] = i;
                    break;
                }
            }
            if (mesh.getDomain().getAxisStartBndType(m) == POLE) {
                if (x(m) < mesh.halfCoords[m](0)) {
                    indices[m][EDGE] = -1;
                    continue;
                } else if (x(m) > mesh.halfCoords[m](mesh.halfCoords[m].size()-1)) {
                    indices[m][EDGE] = mesh.halfCoords[m].size()-1;
                    continue;
                }
            }
            for (int i = 0; i < mesh.halfCoords[m].size()-1; ++i) {
                if (x(m) >= mesh.halfCoords[m](i) && x(m) <= mesh.halfCoords[m](i+1)) {
                    // cout << setw(5) << m << setw(5) << i;
                    // cout << setw(10) << x(m);
                    // cout << setw(10) << mesh.halfCoords[m](i);
                    // cout << setw(10) << mesh.halfCoords[m](i+1) << endl;
                    indices[m][EDGE] = i;
                    break;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}

void StructuredMeshIndex::print() {
    cout << "Center indices: ";
    for (int m = 0; m < 3; ++m) {
        cout << setw(5) << indices[m][CENTER];
    }
    cout << endl;
}
