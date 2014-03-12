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
    assert(mesh.isSet());
    this->mesh = &mesh;
    vr.set_size(mesh.getNumGrid(0, CENTER, true), mesh.getNumGrid(2, CENTER));
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
                       const field<TimeLevels<field<double>, 2>*> &data,
                       bool updateHalfLevel) {
    // ring variable is at A-grids
    int nx = mesh->getNumGrid(0, CENTER, true);
    int j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, CENTER)-2; // off the Pole
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 1; i < mesh->getNumGrid(0, CENTER, true)-1; ++i) {
            vr(i, k)->getLevel(timeIdx)(0) =
                (data(0)->getLevel(timeIdx)(i-1, j, k)+
                 data(0)->getLevel(timeIdx)(  i, j, k))*0.5;
        }
    }
    // periodic boundary condition
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        vr(0, k)->getLevel(timeIdx)(0) = vr(nx-2, k)->getLevel(timeIdx)(0);
        vr(nx-1, k)->getLevel(timeIdx)(0) = vr(1, k)->getLevel(timeIdx)(0);
    }
    j = pole == SOUTH_POLE ? 0 : mesh->getNumGrid(1, EDGE)-2;
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
            vr(i, k)->getLevel(timeIdx)(1) =
                (data(1)->getLevel(timeIdx)(i, j,   k)+
                 data(1)->getLevel(timeIdx)(i, j+1, k))*0.5;
        }
    }
    if (mesh->getDomain().getNumDim() == 3) {
        for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
            for (int i = 0; i < mesh->getNumGrid(0, CENTER, true); ++i) {
                vr(i, k)->getLevel(timeIdx)(2) =
                    (data(2)->getLevel(timeIdx)(i, j, k)+
                     data(2)->getLevel(timeIdx)(i, j, k+1))*0.5;
            }
        }
    }
    // -------------------------------------------------------------------------
    // transform velocity
    double cosLon, sinLon, sinLat, sinLat2;
    j = pole == SOUTH_POLE ? 1 : mesh->getNumGrid(1, CENTER)-2;
    sinLat = mesh->getSinLat(CENTER, j);
    sinLat2 = mesh->getSinLat2(CENTER, j);
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            cosLon = mesh->getCosLon(CENTER, i);
            sinLon = mesh->getSinLon(CENTER, i);
            vr(i+1, k)->getLevel(timeIdx)
                .transformToPS(sinLat, sinLat2, sinLon, cosLon);
        }
    }
    // periodic boundary condition
    for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
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
    for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                cout << setw(10) << setprecision(2);
                cout << getOriginalData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "Transformed velocity:" << endl;
    cout << "dim";
    for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
        cout << setw(10) << i;
    }
    cout << endl;
    for (int m = 0; m < mesh->getDomain().getNumDim(); ++m) {
        cout << setw(2) << m << ":";
        for (int i = 0; i < mesh->getNumGrid(0, CENTER); ++i) {
            for (int k = 0; k < mesh->getNumGrid(2, CENTER); ++k) {
                cout << setw(10) << setprecision(2);
                cout << getTransformedData(m, timeIdx, i, k);
            }
        }
        cout << endl;
    }
}

// -----------------------------------------------------------------------------

RLLVelocityField::RLLVelocityField() : RLLField<double>() {
}

RLLVelocityField::~RLLVelocityField() {
}

void RLLVelocityField::applyBndCond(const TimeLevelIndex<2> &timeIdx,
                                    bool updateHalfLevel) {
    RLLField<double>::applyBndCond(timeIdx, updateHalfLevel);
    rings[0].update(timeIdx, SOUTH_POLE, data, updateHalfLevel);
    rings[1].update(timeIdx, NORTH_POLE, data, updateHalfLevel);
}
    
void RLLVelocityField::create(const string &name, const string &units,
                              const string &longName, const RLLMesh &mesh,
                              int numDim, ArakawaGrid gridType,
                              bool hasHalfLevel) {
    RLLField<double>::create(name, units, longName, mesh, VectorField,
                             numDim, gridType, hasHalfLevel);
    rings[0].create(mesh, hasHalfLevel);
    rings[1].create(mesh, hasHalfLevel);
}

const PolarRing& RLLVelocityField::getPolarRing(Pole pole) const {
    return rings[pole];
}

}
