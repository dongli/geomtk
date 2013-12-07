#include "StructuredVectorField.h"

namespace geomtk {

StructuredVectorField::StructuredVectorField(Mesh &mesh) : Field(mesh) {
    if (dynamic_cast<StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be StructuredMesh!")
    }
    staggerTypes = new StaggerType*[mesh.getDomain().getNumDim()];
    for (int i = 0; i < mesh.getDomain().getNumDim(); ++i) {
        staggerTypes[i] = new StaggerType[mesh.getDomain().getNumDim()];
    }
}

StructuredVectorField::~StructuredVectorField() {
    for (int i = 0; i < mesh->getDomain().getNumDim(); ++i) {
        delete [] staggerTypes[i];
    }
    delete staggerTypes;
}

void StructuredVectorField::applyBndCond(int timeLevel) {
    // zonal periodic boundary condition
    for (int l = 0; l < mesh->getDomain().getNumDim(); ++l) {
        int nx = data[l].getLevel(0).n_rows;
        int ny = data[l].getLevel(0).n_cols;
        int nz = data[l].getLevel(0).n_slices;
        for (int k = 0; k < nz; ++k) {
            for (int j = 0; j < ny; ++j) {
                data[l].getLevel(timeLevel)(0, j, k) = data[l].getLevel(timeLevel)(nx-2, j, k);
                data[l].getLevel(timeLevel)(nx-1, j, k) = data[l].getLevel(timeLevel)(1, j, k);
            }
        }
    }
}

void StructuredVectorField::create(StaggerType uXStaggerType,
                                   StaggerType uYStaggerType,
                                   StaggerType vXStaggerType,
                                   StaggerType vYStaggerType) {
    staggerTypes[0][0] = uXStaggerType;
    staggerTypes[0][1] = uYStaggerType;
    staggerTypes[1][0] = vXStaggerType;
    staggerTypes[1][1] = vYStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].getLevel(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                        mesh.getNumGrid(1, staggerTypes[m][1]), 1);
        }
    }
}

void StructuredVectorField::create(StaggerType uXStaggerType,
                                   StaggerType uYStaggerType,
                                   StaggerType uZStaggerType,
                                   StaggerType vXStaggerType,
                                   StaggerType vYStaggerType,
                                   StaggerType vZStaggerType,
                                   StaggerType wXStaggerType,
                                   StaggerType wYStaggerType,
                                   StaggerType wZStaggerType) {
    if (mesh->getDomain().getNumDim() != 3) {
        REPORT_ERROR("The domain is not 3D!")
    }
    staggerTypes[0][0] = uXStaggerType;
    staggerTypes[0][1] = uYStaggerType;
    staggerTypes[0][2] = uZStaggerType;
    staggerTypes[1][0] = vXStaggerType;
    staggerTypes[1][1] = vYStaggerType;
    staggerTypes[1][2] = vZStaggerType;
    staggerTypes[2][0] = wXStaggerType;
    staggerTypes[2][1] = wYStaggerType;
    staggerTypes[2][2] = wZStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].getLevel(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                        mesh.getNumGrid(1, staggerTypes[m][1]),
                                        mesh.getNumGrid(2, staggerTypes[m][2]));
        }
    }
}

double StructuredVectorField::operator()(int timeLevel, int dim, int i, int j, int k) const {
    // The virtual boundary grids at the periodic boundary conditions are
    // hiden from user.
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
    return data[dim].getLevel(timeLevel)(I, J, k);
}

double& StructuredVectorField::operator()(int timeLevel, int dim, int i, int j, int k) {
    // The virtual boundary grids at the periodic boundary conditions are
    // hiden from user.
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
    return data[dim].getLevel(timeLevel)(I, J, k);
}

StaggerType StructuredVectorField::getStaggerType(int comp, int dim) const {
    return staggerTypes[comp][dim];
}

}
