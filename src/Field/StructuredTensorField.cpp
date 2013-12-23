#include "StructuredTensorField.h"
#include "StructuredMesh.h"

namespace geomtk {

StructuredTensorField::StructuredTensorField(const Mesh &mesh, bool hasHalfLevel)
    : Field(mesh, hasHalfLevel) {
    if (dynamic_cast<const StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be StructuredMesh!");
    }
    data = new TimeLevels<cube, 2>**[mesh.getDomain().getNumDim()];
    for (int m1 = 0; m1 < mesh.getDomain().getNumDim(); ++m1) {
        data[m1] = new TimeLevels<cube, 2>*[mesh.getDomain().getNumDim()];
        for (int m2 = 0; m2 < mesh.getDomain().getNumDim(); ++m2) {
            data[m1][m2] = new TimeLevels<cube, 2>(hasHalfLevel);
        }
    }
}

StructuredTensorField::StructuredTensorField(const string &name,
                                             const string &units,
                                             const string &longName,
                                             const Mesh &mesh,
                                             bool hasHalfLevel)
    : Field(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be StructuredMesh!");
    }
    data = new TimeLevels<cube, 2>**[mesh.getDomain().getNumDim()];
    for (int m1 = 0; m1 < mesh.getDomain().getNumDim(); ++m1) {
        data[m1] = new TimeLevels<cube, 2>*[mesh.getDomain().getNumDim()];
        for (int m2 = 0; m2 < mesh.getDomain().getNumDim(); ++m2) {
            data[m1][m2] = new TimeLevels<cube, 2>(hasHalfLevel);
        }
    }
}

StructuredTensorField::~StructuredTensorField() {
    for (int m1 = 0; m1 < mesh->getDomain().getNumDim(); ++m1) {
        for (int m2 = 0; m2 < mesh->getDomain().getNumDim(); ++m2) {
            delete data[m1][m2];
        }
        delete [] data[m1];
    }
    delete [] data;
}

void StructuredTensorField::applyBndCond(int timeLevel, bool updateHalfLevel) {
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        for (int m1 = 0; m1 < mesh->getDomain().getNumDim(); ++m1) {
            for (int m2 = 0; m2 < mesh->getDomain().getNumDim(); ++m2) {
                int nx = data[m1][m2]->getLevel(0).n_rows;
                int ny = data[m1][m2]->getLevel(0).n_cols;
                int nz = data[m1][m2]->getLevel(0).n_slices;
                for (int k = 0; k < nz; ++k) {
                    for (int j = 0; j < ny; ++j) {
                        data[m1][m2]->getLevel(timeLevel)(0, j, k) =
                            data[m1][m2]->getLevel(timeLevel)(nx-2, j, k);
                        data[m1][m2]->getLevel(timeLevel)(nx-1, j, k) =
                            data[m1][m2]->getLevel(timeLevel)(1, j, k);
                    }
                }
            }
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
    if (updateHalfLevel && data[0][0]->hasHalfLevel()) {
        for (int m1 = 0; m1 < mesh->getDomain().getNumDim(); ++m1) {
            for (int m2 = 0; m2 < mesh->getDomain().getNumDim(); ++m2) {
                data[m1][m2]->updateHalfLevel();
            }
        }
    }
}

void StructuredTensorField::create() {
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0][0]->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
        for (int m1 = 0; m1 < mesh.getDomain().getNumDim(); ++m1) {
            for (int m2 = 0; m2 < mesh.getDomain().getNumDim(); ++m2) {
                data[m1][m2]->getLevel(l).reshape(mesh.getNumGrid(0, CENTER, true),
                                                  mesh.getNumGrid(1, CENTER),
                                                  mesh.getNumGrid(2, CENTER));
            }
        }
    }
}

void StructuredTensorField::calcFromVector(const StructuredVectorField &vector,
                                           int timeLevel) {
//    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    // divergence part
    
}

double StructuredTensorField::operator()(int timeLevel, int dim1, int dim2,
                                         int i, int j, int k) const {
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data[dim1][dim2]->getLevel(timeLevel)(I, J, k);
}

double& StructuredTensorField::operator()(int timeLevel, int dim1, int dim2,
                                          int i, int j, int k) {
    int I, J;
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        I = i+1;
    } else {
        I = i;
    }
    if (mesh->getDomain().getAxisStartBndType(1) == PERIODIC) {
        J = j+1;
    } else {
        J = j;
    }
    return data[dim1][dim2]->getLevel(timeLevel)(I, J, k);
}

}