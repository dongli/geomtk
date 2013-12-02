#include "StructuredVectorField.h"

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
        int nx = data[l].get(0).n_rows;
        int ny = data[l].get(0).n_cols;
        int nz = data[l].get(0).n_slices;
        for (int k = 0; k < nz; ++k) {
            for (int j = 0; j < ny; ++j) {
                data[l].get(timeLevel)(0, j, k) = data[l].get(timeLevel)(nx-2, j, k);
                data[l].get(timeLevel)(nx-1, j, k) = data[l].get(timeLevel)(1, j, k);
            }
        }
    }
}

void StructuredVectorField::create(StaggerType xLonStaggerType,
                                   StaggerType xLatStaggerType,
                                   StaggerType yLonStaggerType,
                                   StaggerType yLatStaggerType) {
    staggerTypes[0][0] = xLonStaggerType;
    staggerTypes[0][1] = xLatStaggerType;
    staggerTypes[1][0] = yLonStaggerType;
    staggerTypes[1][1] = yLatStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].get(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                   mesh.getNumGrid(1, staggerTypes[m][1]), 1);
        }
    }
}

void StructuredVectorField::create(StaggerType xLonStaggerType,
                                   StaggerType xLatStaggerType,
                                   StaggerType xLevStaggerType,
                                   StaggerType yLonStaggerType,
                                   StaggerType yLatStaggerType,
                                   StaggerType yLevStaggerType,
                                   StaggerType zLonStaggerType,
                                   StaggerType zLatStaggerType,
                                   StaggerType zLevStaggerType) {
    if (mesh->getDomain().getNumDim() != 3) {
        REPORT_ERROR("The domain is not 3D!")
    }
    staggerTypes[0][0] = xLonStaggerType;
    staggerTypes[0][1] = xLatStaggerType;
    staggerTypes[0][2] = xLevStaggerType;
    staggerTypes[1][0] = yLonStaggerType;
    staggerTypes[1][1] = yLatStaggerType;
    staggerTypes[1][2] = yLevStaggerType;
    staggerTypes[2][0] = zLonStaggerType;
    staggerTypes[2][1] = zLatStaggerType;
    staggerTypes[2][2] = zLevStaggerType;
    const StructuredMesh &mesh = dynamic_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0].getNumLevel(); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m].get(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                   mesh.getNumGrid(1, staggerTypes[m][1]),
                                   mesh.getNumGrid(2, staggerTypes[m][2]));
        }
    }
}

double StructuredVectorField::operator()(int timeLevel, int dim, int i, int j, int k) const {
    return data[dim].get(timeLevel)(i+1, j, k);
}

double& StructuredVectorField::operator()(int timeLevel, int dim, int i, int j, int k) {
    return data[dim].get(timeLevel)(i+1, j, k);
}

StaggerType StructuredVectorField::getStaggerType(int comp, int dim) const {
    return staggerTypes[comp][dim];
}
