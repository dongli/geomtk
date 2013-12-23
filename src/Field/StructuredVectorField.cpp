#include "StructuredVectorField.h"

namespace geomtk {

StructuredVectorField::StructuredVectorField(const Mesh &mesh, bool hasHalfLevel)
    : Field(mesh, hasHalfLevel) {
    if (dynamic_cast<const StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be StructuredMesh!");
    }
    for (int i = 0; i < 3; ++i) {
        data[i] = new TimeLevels<cube, 2>(hasHalfLevel);
    }
    staggerTypes = new StaggerType*[mesh.getDomain().getNumDim()];
    for (int i = 0; i < mesh.getDomain().getNumDim(); ++i) {
        staggerTypes[i] = new StaggerType[mesh.getDomain().getNumDim()];
    }
}
    
StructuredVectorField::StructuredVectorField(const string &name,
                                             const string &units,
                                             const string &longName,
                                             const Mesh &mesh,
                                             bool hasHalfLevel)
    : Field(name, units, longName, mesh, hasHalfLevel) {
    if (dynamic_cast<const StructuredMesh*>(&mesh) == NULL) {
        REPORT_ERROR("Mesh should be StructuredMesh!");
    }
    for (int i = 0; i < 3; ++i) {
        data[i] = new TimeLevels<cube, 2>(hasHalfLevel);
    }
    staggerTypes = new StaggerType*[mesh.getDomain().getNumDim()];
    for (int i = 0; i < mesh.getDomain().getNumDim(); ++i) {
        staggerTypes[i] = new StaggerType[mesh.getDomain().getNumDim()];
    }
}

StructuredVectorField::~StructuredVectorField() {
    for (int i = 0; i < 3; ++i) {
        delete data[i];
    }
    for (int i = 0; i < mesh->getDomain().getNumDim(); ++i) {
        delete [] staggerTypes[i];
    }
    delete staggerTypes;
}

void StructuredVectorField::applyBndCond(int timeLevel, bool updateHalfLevel) {
    // zonal periodic boundary condition
    if (mesh->getDomain().getAxisStartBndType(0) == PERIODIC) {
        for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
            int nx = data[m]->getLevel(0).n_rows;
            int ny = data[m]->getLevel(0).n_cols;
            int nz = data[m]->getLevel(0).n_slices;
            for (int k = 0; k < nz; ++k) {
                for (int j = 0; j < ny; ++j) {
                    data[m]->getLevel(timeLevel)(0, j, k) =
                        data[m]->getLevel(timeLevel)(nx-2, j, k);
                    data[m]->getLevel(timeLevel)(nx-1, j, k) =
                        data[m]->getLevel(timeLevel)(1, j, k);
                }
            }
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
    // update half level
    if (updateHalfLevel && data[0]->hasHalfLevel()) {
        for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
            data[m]->updateHalfLevel();
        }
    }
}
    
void StructuredVectorField::create(geomtk::ArakawaGrid gridType) {
    this->gridType = gridType;
    switch (gridType) {
        case A_GRID:
            REPORT_ERROR("Under construction!");
            break;
        case B_GRID:
            REPORT_ERROR("Under construction!");
            break;
        case C_GRID:
            if (mesh->getDomain().getNumDim() == 2) {
                create(EDGE, CENTER, CENTER, EDGE);
            } else if (mesh->getDomain().getNumDim() == 3) {
                create(EDGE, CENTER, CENTER, CENTER, EDGE, CENTER,
                       CENTER, CENTER, EDGE);
            }
            break;
        case D_GRID:
            REPORT_ERROR("Under construction!");
            break;
        case E_GRID:
            REPORT_ERROR("Under construction!");
            break;
    }
}

void StructuredVectorField::create(StaggerType uXStaggerType,
                                   StaggerType uYStaggerType,
                                   StaggerType vXStaggerType,
                                   StaggerType vYStaggerType) {
    if (uXStaggerType == CENTER && uYStaggerType == CENTER &&
        vXStaggerType == CENTER && vYStaggerType == CENTER) {
        gridType = A_GRID;
    } else if (uXStaggerType == EDGE && uYStaggerType == CENTER &&
               vXStaggerType == CENTER && vYStaggerType == EDGE) {
        gridType = C_GRID;
    } else {
        REPORT_ERROR("Under construction!");
    }
    staggerTypes[0][0] = uXStaggerType;
    staggerTypes[0][1] = uYStaggerType;
    staggerTypes[1][0] = vXStaggerType;
    staggerTypes[1][1] = vYStaggerType;
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    for (int l = 0; l < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m]->getLevel(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
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
    if (uXStaggerType == CENTER && uYStaggerType == CENTER && uZStaggerType == CENTER &&
        vXStaggerType == CENTER && vYStaggerType == CENTER && vZStaggerType == CENTER &&
        wXStaggerType == CENTER && wYStaggerType == CENTER && wZStaggerType == CENTER) {
        gridType = A_GRID;
    } else if (uXStaggerType == EDGE && uYStaggerType == CENTER && uZStaggerType == CENTER &&
               vXStaggerType == CENTER && vYStaggerType == EDGE && vZStaggerType == CENTER &&
               wXStaggerType == CENTER && wYStaggerType == CENTER && wZStaggerType == EDGE) {
        gridType = C_GRID;
    } else {
        REPORT_ERROR("Under construction!");
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
    for (int l = 0; l < data[0]->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
        for (int m = 0; m < mesh.getDomain().getNumDim(); ++m) {
            data[m]->getLevel(l).reshape(mesh.getNumGrid(0, staggerTypes[m][0], true),
                                         mesh.getNumGrid(1, staggerTypes[m][1]),
                                         mesh.getNumGrid(2, staggerTypes[m][2]));
        }
    }
}

void StructuredVectorField::convert(ArakawaGrid gridType, int timeLevel,
                                    cube &u, cube &v) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    if (this->gridType == C_GRID && gridType == A_GRID) {
        if (u.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("");
        } else if (u.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("");
        }
        if (v.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("");
        } else if (v.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("");
        }
        if (mesh.getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                    u(i, j, 0) = ((*this)(timeLevel, 0, i-1, j)+
                                  (*this)(timeLevel, 0,   i, j))*0.5;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(1) == POLE) {
            for (int j = 1; j < mesh.getNumGrid(1, CENTER)-1; ++j) {
                for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                    v(i, j, 0) = ((*this)(timeLevel, 1, i,   j)+
                                  (*this)(timeLevel, 1, i, j-1))*0.5;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
}

void StructuredVectorField::convert(ArakawaGrid gridType, int timeLevel,
                                    cube u, cube v, cube w) {
    const StructuredMesh &mesh = static_cast<const StructuredMesh&>(*(this->mesh));
    if (this->gridType == C_GRID && gridType == A_GRID) {
        if (u.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("");
        } else if (u.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("");
        } else if (u.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("");
        }
        if (v.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("");
        } else if (v.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("");
        } else if (v.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("");
        }
        if (w.n_rows != mesh.getNumGrid(0, CENTER)) {
            REPORT_ERROR("");
        } else if (w.n_cols != mesh.getNumGrid(1, CENTER)) {
            REPORT_ERROR("");
        } else if (w.n_slices != mesh.getNumGrid(2, CENTER)) {
            REPORT_ERROR("");
        }
        if (mesh.getDomain().getAxisStartBndType(0) == PERIODIC) {
            for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
                for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        u(i, j, k) = (data[0]->getLevel(timeLevel)(  i, j, k)+
                                      data[0]->getLevel(timeLevel)(i+1, j, k))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(1) == POLE) {
            for (int k = 0; k < mesh.getNumGrid(2, CENTER); ++k) {
                for (int j = 1; j < mesh.getNumGrid(1, CENTER)-1; ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        v(i, j, k) = (data[1]->getLevel(timeLevel)(i,   j, k)+
                                      data[1]->getLevel(timeLevel)(i, j-1, k))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (mesh.getDomain().getAxisStartBndType(2) == RIGID) {
            for (int k = 1; k < mesh.getNumGrid(2, CENTER)-1; ++k) {
                for (int j = 0; j < mesh.getNumGrid(1, CENTER); ++j) {
                    for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
                        w(i, j, k) = (data[2]->getLevel(timeLevel)(i, j, k)+
                                      data[2]->getLevel(timeLevel)(i, j, k-1))*0.5;
                    }
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
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
    return data[dim]->getLevel(timeLevel)(I, J, k);
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
    return data[dim]->getLevel(timeLevel)(I, J, k);
}

ArakawaGrid StructuredVectorField::getGridType() const {
    return gridType;
}

StaggerType StructuredVectorField::getStaggerType(int comp, int dim) const {
    return staggerTypes[comp][dim];
}

}
