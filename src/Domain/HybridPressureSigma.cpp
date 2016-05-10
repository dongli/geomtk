#include "HybridPressureSigma.h"

namespace geomtk {

HybridPressureSigma::HybridPressureSigma() {
    _type = HYBRID_PRESSURE_SIGMA;
}

HybridPressureSigma::~HybridPressureSigma() {

}
    
void HybridPressureSigma::
    init(int size) {
        A[GridType::FULL].set_size(size);
        A[GridType::HALF].set_size(size+1);
        B[GridType::FULL].set_size(size);
        B[GridType::HALF].set_size(size+1);
    }

double HybridPressureSigma::
calcPressure(int gridType, double ps, int k) const {
    double p;
    p = A[gridType](k)*p0+B[gridType](k)*ps;
    return p;
}

}
