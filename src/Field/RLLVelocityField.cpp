#include "RLLVelocityField.h"

namespace geomtk {

PolarRing::PolarRing() {
}

PolarRing::~PolarRing() {
    if (vr.size() > 0) {
        for (int k = 0; k < vr.n_cols; ++k) {
            for (int i = 0; i < vr.n_rows; ++i) {
                delete vr(i, k);
                delete divr(i, k);
            }
        }
    }
}

void PolarRing::create(const RLLMesh &mesh, bool hasHalfLevel) {
    this->mesh = &mesh;
    vr.set_size(mesh.getNumGrid(0, GridType::FULL, true),
                mesh.getNumGrid(2, GridType::FULL));
    divr.set_size(mesh.getNumGrid(0, GridType::FULL, true),
                  mesh.getNumGrid(2, GridType::FULL));
    for (int k = 0; k < vr.n_cols; ++k) {
        for (int i = 0; i < vr.n_rows; ++i) {
            vr(i, k) = new TimeLevels<SphereVelocity, 2>(hasHalfLevel);
            for (int l = 0; l < vr(i, k)->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
                vr(i, k)->getLevel(l).setNumDim(mesh.getDomain().getNumDim());
            }
            divr(i, k) = new TimeLevels<double, 2>(hasHalfLevel);
        }
    }
}

void PolarRing::update(const TimeLevelIndex<2> &timeIdx, Pole pole,
                       const vector<RLLField<double, 2> > &v,
                       const RLLField<double, 2> &div,
                       bool updateHalfLevel) {
    // Ring variable is at A-grids.
    // Set zonal wind speed.
    int j = pole == SOUTH_POLE ? mesh->js(GridType::FULL)+1 : mesh->je(GridType::FULL)-1; // off the Pole
    if (v[0].getStaggerLocation() == Location::X_FACE) { // C-grid
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                vr(i, k)->getLevel(timeIdx)(0) =
                    (v[0](timeIdx, i-1, j, k)+v[0](timeIdx, i, j, k))*0.5;
            }
        }
        // periodic boundary condition
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            vr(mesh->is(GridType::FULL)-1, k)->getLevel(timeIdx)(0) =
            vr(mesh->ie(GridType::FULL), k)->getLevel(timeIdx)(0);
            vr(mesh->ie(GridType::FULL)+1, k)->getLevel(timeIdx)(0) =
            vr(mesh->is(GridType::FULL), k)->getLevel(timeIdx)(0);
        }
    } else if (v[0].getStaggerLocation() == Location::CENTER) { // A-grid
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
                vr(i, k)->getLevel(timeIdx)(0) = v[0](timeIdx, i, j, k);
            }
        }
    }
    // Set vertical wind speed.
    if (mesh->getDomain().getNumDim() == 3) {
        if (v[2].getStaggerLocation() == Location::Z_FACE) { // C-grid
            for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
                for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
                    vr(i, k)->getLevel(timeIdx)(2) =
                        (v[2](timeIdx, i, j, k)+v[2](timeIdx, i, j, k+1))*0.5;
                }
            }
        } else if (v[2].getStaggerLocation() == Location::CENTER) { // A-grid
            for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
                for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
                    vr(i, k)->getLevel(timeIdx)(2) = v[2](timeIdx, i, j, k);
                }
            }
        }
    }
    // Set divergence.
    for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
        for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
            divr(i, k)->getLevel(timeIdx) = div(timeIdx, i, j, k);
        }
    }
    // Set meridional wind speed.
    j = pole == SOUTH_POLE ? mesh->js(GridType::HALF) : mesh->je(GridType::HALF)-1;
    if (v[1].getStaggerLocation() == Location::Y_FACE) { // C-grid
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
                vr(i, k)->getLevel(timeIdx)(1) =
                    (v[1](timeIdx, i, j, k)+v[1](timeIdx, i, j+1, k))*0.5;
            }
        }
    } else if (v[1].getStaggerLocation() == Location::CENTER) { // A-grid
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
                vr(i, k)->getLevel(timeIdx)(1) = v[1](timeIdx, i, j, k);
            }
        }
    }
    // Transform velocity.
    double cosLon, sinLon, sinLat, sinLat2;
    j = pole == SOUTH_POLE ? mesh->js(GridType::FULL)+1 : mesh->je(GridType::FULL)-1; // off the Pole
    sinLat = mesh->getSinLat(GridType::FULL, j);
    sinLat2 = mesh->getSinLat2(GridType::FULL, j);
    for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
        for (int i = mesh->is(GridType::FULL)-1; i <= mesh->ie(GridType::FULL)+1; ++i) {
            cosLon = mesh->getCosLon(GridType::FULL, i);
            sinLon = mesh->getSinLon(GridType::FULL, i);
            vr(i, k)->getLevel(timeIdx).transformToPS(sinLat, sinLat2, sinLon, cosLon);
        }
    }
    // Update half level.
    if (updateHalfLevel && vr(0, 0)->hasHalfLevel()) {
        TimeLevelIndex<2> halfTimeIdx = timeIdx-0.5;
        TimeLevelIndex<2> oldTimeIdx = timeIdx-1;
        for (int k = 0; k < vr.n_cols; ++k) {
            for (int i = 0; i < vr.n_rows; ++i) {
                vr(i, k)->getLevel(halfTimeIdx) =
                    (vr(i, k)->getLevel(oldTimeIdx)+
                     vr(i, k)->getLevel(timeIdx))*0.5;
            }
        }
    }
}

double PolarRing::
getOriginalData(int dim, const TimeLevelIndex<2> &timeIdx, int i, int k) const {
    return vr(i, k)->getLevel(timeIdx)(dim);
}

double PolarRing::
getTransformedData(int dim, const TimeLevelIndex<2> &timeIdx,
                   int i, int k) const {
    return vr(i, k)->getLevel(timeIdx)[dim];
}

void PolarRing::print() const {
    TimeLevelIndex<2> timeIdx;
    cout << "---------------------------------------------------------" << endl;
    cout << "Reorganized original velocity:" << endl;
    cout << "dim";
    for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                cout << setw(10) << setprecision(2);
                cout << getOriginalData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "Transformed velocity:" << endl;
    cout << "dim";
    for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int k = mesh->ks(GridType::FULL); k <= mesh->ke(GridType::FULL); ++k) {
            for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                cout << setw(10) << setprecision(2);
                cout << getTransformedData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
}

// -----------------------------------------------------------------------------

RLLVelocityField::RLLVelocityField()
    : StructuredVelocityField<MeshType, FieldType>() {
}

RLLVelocityField::~RLLVelocityField() {
}

void RLLVelocityField::create(const RLLMesh &mesh, bool useStagger,
                              bool hasHalfLevel) {
    StructuredVelocityField<MeshType, FieldType>::
    create(mesh, useStagger, hasHalfLevel);
    v[0].setLongName("zonal wind speed");
    v[1].setLongName("meridional wind speed");
    if (mesh.getDomain().getNumDim() == 3) {
        v[2].setLongName("vertical wind speed");
    }
    rings[0].create(mesh, hasHalfLevel);
    rings[1].create(mesh, hasHalfLevel);
}

void RLLVelocityField::applyBndCond(const TimeLevelIndex<2> &timeIdx,
                                    bool updateHalfLevel) {
    StructuredVelocityField<MeshType, FieldType>::
    applyBndCond(timeIdx, updateHalfLevel);
    rings[0].update(timeIdx, SOUTH_POLE, v, div, updateHalfLevel);
    rings[1].update(timeIdx, NORTH_POLE, v, div, updateHalfLevel);
}

void RLLVelocityField::calcDivergence(const TimeLevelIndex<2> &timeIdx) {
    if (v[0].getStaggerLocation() == Location::CENTER &&
        v[1].getStaggerLocation() == Location::CENTER) {
        for (int j = mesh->js(GridType::FULL)+1; j <= mesh->je(GridType::FULL)-1; ++j) {
            double ReCosLat = mesh->getDomain().getRadius()*mesh->getCosLat(GridType::FULL, j);
            for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                double u1 = v[0](timeIdx, i-1, j);
                double u2 = v[0](timeIdx, i+1, j);
                double dudlon = (u2-u1)/(2*mesh->getGridInterval(0, GridType::HALF, 0));
                double vCosLat1 = v[1](timeIdx, i, j-1)*mesh->getCosLat(GridType::FULL, j-1);
                double vCosLat2 = v[1](timeIdx, i, j+1)*mesh->getCosLat(GridType::FULL, j+1);
                double dlat = (mesh->getGridInterval(1, GridType::FULL, j-1)+
                               mesh->getGridInterval(1, GridType::FULL, j));
                double dvCosLatdlat = (vCosLat2-vCosLat1)/dlat;
                div(timeIdx, i, j) = (dudlon+dvCosLatdlat)/ReCosLat;
            }
        }
    } else if (v[0].getStaggerLocation() == Location::X_FACE &&
               v[1].getStaggerLocation() == Location::Y_FACE) {
        for (int j = mesh->js(GridType::FULL)+1; j <= mesh->je(GridType::FULL)-1; ++j) {
            double ReCosLat = mesh->getDomain().getRadius()*mesh->getCosLat(GridType::FULL, j);
            for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                double u1 = v[0](timeIdx, i-1, j);
                double u2 = v[0](timeIdx, i, j);
                double dudlon = (u2-u1)/mesh->getGridInterval(0, GridType::HALF, 0);
                double vCosLat1 = v[1](timeIdx, i, j-1)*mesh->getCosLat(GridType::HALF, j-1);
                double vCosLat2 = v[1](timeIdx, i, j)*mesh->getCosLat(GridType::HALF, j);
                double dvCosLatdlat = (vCosLat2-vCosLat1)/mesh->getGridInterval(1, GridType::HALF, j);
                div(timeIdx, i, j) = (dudlon+dvCosLatdlat)/ReCosLat;
            }
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
}
    
void RLLVelocityField::calcVorticity(const TimeLevelIndex<2> &timeIdx) {
    if (v[0].getStaggerLocation() == Location::CENTER &&
        v[1].getStaggerLocation() == Location::CENTER) {
        if (mesh->getDomain().getNumDim() == 2) {
            for (int j = mesh->js(GridType::FULL)+1; j <= mesh->je(GridType::FULL)-1; ++j) {
                double ReCosLat = mesh->getDomain().getRadius()*mesh->getCosLat(GridType::FULL, j);
                for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                    double v1 = v[1](timeIdx, i-1, j);
                    double v2 = v[1](timeIdx, i, j);
                    double dvdlon = (v2-v1)/(2*mesh->getGridInterval(0, GridType::HALF, 0));
                    double uCosLat1 = v[0](timeIdx, i, j-1)*mesh->getCosLat(GridType::FULL, j-1);
                    double uCosLat2 = v[0](timeIdx, i, j+1)*mesh->getCosLat(GridType::FULL, j+1);
                    double dlat = (mesh->getGridInterval(1, GridType::FULL, j-1)+
                                   mesh->getGridInterval(1, GridType::FULL, j));
                    double duCosLatdlat = (uCosLat2-uCosLat1)/dlat;
                    vor[0](timeIdx, i, j) = (dvdlon-duCosLatdlat)/ReCosLat;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    } else if (v[0].getStaggerLocation() == Location::X_FACE &&
               v[1].getStaggerLocation() == Location::Y_FACE) {
        if (mesh->getDomain().getNumDim() == 2) {
            for (int j = mesh->js(GridType::FULL)+1; j <= mesh->je(GridType::FULL)-1; ++j) {
                double ReCosLat = mesh->getDomain().getRadius()*mesh->getCosLat(GridType::FULL, j);
                for (int i = mesh->is(GridType::FULL); i <= mesh->ie(GridType::FULL); ++i) {
                    double v1 = v[1](timeIdx, i-1, j-1);
                    double v2 = v[1](timeIdx, i-1, j );
                    double v3 = v[1](timeIdx, i+1, j-1);
                    double v4 = v[1](timeIdx, i+1, j  );
                    double dvdlon = (v3+v4-v1-v2)*0.5/(2*mesh->getGridInterval(0, GridType::HALF, 0));
                    double uCosLat1 = v[0](timeIdx, i-1, j-1)*mesh->getCosLat(GridType::HALF, j-1);
                    double uCosLat2 = v[0](timeIdx, i,   j-1)*mesh->getCosLat(GridType::HALF, j-1);
                    double uCosLat3 = v[0](timeIdx, i-1, j+1)*mesh->getCosLat(GridType::HALF, j);
                    double uCosLat4 = v[0](timeIdx, i,   j+1)*mesh->getCosLat(GridType::HALF, j);
                    double dlat = (mesh->getGridInterval(1, GridType::FULL, j-1)+
                                   mesh->getGridInterval(1, GridType::FULL, j));
                    double duCosLatdlat = (uCosLat4+uCosLat3-uCosLat2-uCosLat1)*0.5/dlat;
                    vor[0](timeIdx, i, j) = (dvdlon-duCosLatdlat)/ReCosLat;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    } else {
        REPORT_ERROR("Under construction!");
    }
}

} // geomtk
