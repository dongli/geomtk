#include "RLLVelocityField.h"

namespace geomtk {

PolarRing::PolarRing() {
}

PolarRing::~PolarRing() {
    if (vr.size() > 0) {
        for (int k = 0; k < vr.n_cols; ++k) {
            for (int i = 0; i < vr.n_rows; ++i) {
                delete vr(i, k);
            }
        }
    }
}

void PolarRing::create(const RLLMesh &mesh, bool hasHalfLevel) {
    this->mesh = &mesh;
    vr.set_size(mesh.getNumGrid(0, GridType::FULL, true),
                mesh.getNumGrid(2, GridType::FULL));
    for (int k = 0; k < vr.n_cols; ++k) {
        for (int i = 0; i < vr.n_rows; ++i) {
            vr(i, k) = new TimeLevels<SphereVelocity, 2>(hasHalfLevel);
            for (int l = 0; l < vr(i, k)->getNumLevel(INCLUDE_HALF_LEVEL); ++l) {
                vr(i, k)->getLevel(l).setNumDim(mesh.getDomain().getNumDim());
            }
        }
    }
}

void PolarRing::update(const TimeLevelIndex<2> &timeIdx, Pole pole,
                       const vector<RLLField<double> > &v,
                       bool updateHalfLevel) {
    // ring variable is at A-grids
    int nx = mesh->getNumGrid(0, GridType::FULL, true);
    // zonal wind speed
    int j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, GridType::FULL)-2; // off the Pole
    if (v[0].getStaggerLocation() == Location::X_FACE) { // C-grid
        for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
            for (int i = 1; i < mesh->getNumGrid(0, GridType::HALF)+1; ++i) {
                vr(i, k)->getLevel(timeIdx)(0) =
                    (v[0](timeIdx, i-1, j, k)+v[0](timeIdx, i, j, k))*0.5;
            }
        }
        // periodic boundary condition
        for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
            vr(0, k)->getLevel(timeIdx)(0) = vr(nx-2, k)->getLevel(timeIdx)(0);
            vr(nx-1, k)->getLevel(timeIdx)(0) = vr(1, k)->getLevel(timeIdx)(0);
        }
    } else if (v[0].getStaggerLocation() == Location::CENTER) { // A-grid
        for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
            for (int i = -1; i < mesh->getNumGrid(0, GridType::FULL)+1; ++i) {
                vr(i+1, k)->getLevel(timeIdx)(0) = v[0](timeIdx, i, j, k);
            }
        }
    }
    // meridional wind speed
    j = pole == SOUTH_POLE ? 0 : mesh->getNumGrid(1, GridType::HALF)-2;
    if (v[1].getStaggerLocation() == Location::Y_FACE) {
        for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
            for (int i = -1; i < mesh->getNumGrid(0, GridType::FULL)+1; ++i) {
                vr(i+1, k)->getLevel(timeIdx)(1) =
                    (v[1](timeIdx, i, j, k)+v[1](timeIdx, i, j+1, k))*0.5;
            }
        }
    } else if (v[1].getStaggerLocation() == Location::CENTER) {
        for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
            for (int i = -1; i < mesh->getNumGrid(0, GridType::FULL)+1; ++i) {
                vr(i+1, k)->getLevel(timeIdx)(1) = v[1](timeIdx, i, j, k);
            }
        }
    }
    // vertical wind speed
    if (mesh->getDomain().getNumDim() == 3) {
        if (v[2].getStaggerLocation() == Location::Z_FACE) {
            for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
                for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL, true); ++i) {
                    vr(i, k)->getLevel(timeIdx)(2) =
                        (v[2](timeIdx, i, j, k)+v[2](timeIdx, i, j, k+1))*0.5;
                }
            }
        } else if (v[2].getStaggerLocation() == Location::CENTER) {
            
        }
    }
    // -------------------------------------------------------------------------
    // transform velocity
    double cosLon, sinLon, sinLat, sinLat2;
    j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, GridType::FULL)-2;
    sinLat = mesh->getSinLat(GridType::FULL, j);
    sinLat2 = mesh->getSinLat2(GridType::FULL, j);
    for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
        for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
            cosLon = mesh->getCosLon(GridType::FULL, i);
            sinLon = mesh->getSinLon(GridType::FULL, i);
            vr(i+1, k)->getLevel(timeIdx)
                .transformToPS(sinLat, sinLat2, sinLon, cosLon);
        }
    }
    // periodic boundary condition
    for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
        for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
            vr(0, k)->getLevel(timeIdx)[m] = vr(nx-2, k)->getLevel(timeIdx)[m];
            vr(nx-1, k)->getLevel(timeIdx)[m] = vr(1, k)->getLevel(timeIdx)[m];
        }
    }
    // -------------------------------------------------------------------------
    // update half level
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

double PolarRing::getOriginalData(int dim, const TimeLevelIndex<2> &timeIdx,
                                  int i, int k) const {
    return vr(i+1, k)->getLevel(timeIdx)(dim);
}

double PolarRing::getTransformedData(int dim, const TimeLevelIndex<2> &timeIdx,
                                     int i, int k) const {
    return vr(i+1, k)->getLevel(timeIdx)[dim];
}

void PolarRing::print() const {
    TimeLevelIndex<2> timeIdx;
    cout << "---------------------------------------------------------" << endl;
    cout << "Reorganized original velocity:" << endl;
    cout << "dim";
    for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
                cout << setw(10) << setprecision(2);
                cout << getOriginalData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "Transformed velocity:" << endl;
    cout << "dim";
    for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, GridType::FULL); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, GridType::FULL); ++k) {
                cout << setw(10) << setprecision(2);
                cout << getTransformedData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
}

// -----------------------------------------------------------------------------

RLLVelocityField::RLLVelocityField() {
}

RLLVelocityField::~RLLVelocityField() {
}

void RLLVelocityField::applyBndCond(const TimeLevelIndex<2> &timeIdx,
                                    bool updateHalfLevel) {
    for (int m = 0; m < v.size(); ++m) {
        v[m].applyBndCond(timeIdx, updateHalfLevel);
    }
    rings[0].update(timeIdx, SOUTH_POLE, v, updateHalfLevel);
    rings[1].update(timeIdx, NORTH_POLE, v, updateHalfLevel);
}
    
void RLLVelocityField::create(const RLLMesh &mesh, bool useStagger,
                              bool hasHalfLevel) {
    v.resize(mesh.getDomain().getNumDim());
    if (useStagger) {
        v[0].create("u", "m s-1", "zonal wind speed", mesh, Location::X_FACE, hasHalfLevel);
        v[1].create("v", "m s-1", "meridional wind speed", mesh, Location::Y_FACE, hasHalfLevel);
        if (mesh.getDomain().getNumDim() == 3) {
            REPORT_ERROR("Under construction!");
            v[2].create("w", "?", "vertical wind speed", mesh, Location::Z_FACE, hasHalfLevel);
        }
    } else {
        v[0].create("u", "m s-1", "zonal wind speed", mesh, Location::CENTER, hasHalfLevel);
        v[1].create("v", "m s-1", "meridional wind speed", mesh, Location::CENTER, hasHalfLevel);
        if (mesh.getDomain().getNumDim() == 3) {
            REPORT_ERROR("Under construction!");
            v[2].create("w", "?", "vertical wind speed", mesh, Location::CENTER, hasHalfLevel);
        }
    }
    rings[0].create(mesh, hasHalfLevel);
    rings[1].create(mesh, hasHalfLevel);
}

const PolarRing& RLLVelocityField::getPolarRing(Pole pole) const {
    return rings[pole];
}

}
