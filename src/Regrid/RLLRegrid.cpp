#include "RLLRegrid.h"
#include "RLLScalarField.h"
#include "RLLVectorField.h"

namespace geomtk {

RLLRegrid::RLLRegrid(const Mesh &mesh) : Regrid(mesh) {
}

RLLRegrid::~RLLRegrid() {
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                    const SpaceCoord &x, double &y, MeshIndex *idx_) {
    const RLLScalarField *g = dynamic_cast<const RLLScalarField*>(&f);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    if (g == NULL) {
        REPORT_ERROR("Mesh should comply RLLScalarField!");
    }
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(*(this->mesh), x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation

    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, (*g).getStaggerType(0));
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, (*g).getStaggerType(1));
            j2 = j1; j3 = j1+1; j4 = j3;
            // cout << "----------------------------------" << endl;
            // cout << " i1 =" << setw(10) << i1 << endl;
            // cout << " i2 =" << setw(10) << i2 << endl;
            // cout << " j1 =" << setw(10) << j1 << endl;
            // cout << " j2 =" << setw(10) << j2 << endl;
            double x1 = mesh.getGridCoord(0, g->getStaggerType(0), i1);
            double x2 = mesh.getGridCoord(0, g->getStaggerType(0), i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh.getGridCoord(1, g->getStaggerType(1), j1);
            double y2 = mesh.getGridCoord(1, g->getStaggerType(1), j3);
            double Y = (x(1)-y1)/(y2-y1);
            // cout << "----------------------------------" << endl;
            // cout << " x1 =" << setw(10) << x1 << endl;
            // cout << " x2 =" << setw(10) << x2 << endl;
            // cout << " y1 =" << setw(10) << y1 << endl;
            // cout << " y2 =" << setw(10) << y2 << endl;
            // cout << "----------------------------------" << endl;
            // cout << " X  =" << setw(10) << X << endl;
            // cout << " Y  =" << setw(10) << Y << endl;
            double f1 = (*g)(timeLevel, i1, j1, 0);
            double f2 = (*g)(timeLevel, i2, j2, 0);
            double f3 = (*g)(timeLevel, i3, j3, 0);
            double f4 = (*g)(timeLevel, i4, j4, 0);
            // cout << "----------------------------------" << endl;
            // cout << " f1 =" << setw(10) << f1 << endl;
            // cout << " f2 =" << setw(10) << f2 << endl;
            // cout << " f3 =" << setw(10) << f3 << endl;
            // cout << " f4 =" << setw(10) << f4 << endl;
            double a = f1;
            double b = f2-f1;
            double c = f3-f1;
            double d = f1-f2-f3+f4;
            // cout << "----------------------------------" << endl;
            // cout << " a  =" << setw(10) << a << endl;
            // cout << " b  =" << setw(10) << b << endl;
            // cout << " c  =" << setw(10) << c << endl;
            // cout << " d  =" << setw(10) << d << endl;
            y = a+b*X+c*Y+d*X*Y;
            // cout << "----------------------------------" << endl;
            // cout << " y(" << m << ") =" << setw(10) << y(m) << endl;
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                    const SpaceCoord &x, vec &y, MeshIndex *idx_) {
    const RLLVectorField *g = dynamic_cast<const RLLVectorField*>(&f);
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    if (g == NULL) {
        REPORT_ERROR("Mesh should comply RLLVectorField!");
    }
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(mesh, x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    if (idx->isInPolarCap()) {
        
    } else {

    }
    if (idx_ == NULL) {
        delete idx;
    }
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const RLLVelocityField &f,
                    const SpaceCoord &x, SphereVelocity &y, RLLMeshIndex *idx_) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(mesh, x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    // cout << " x(0) =" << setw(10) << x(0) << endl;
    // cout << " x(1) =" << setw(10) << x(1) << endl;
    // idx->print();
    if (idx->isInPolarCap()) {
        const SphereDomain &domain = static_cast<const SphereDomain&>(mesh.getDomain());
        const PolarRing &ring = f.getPolarRing(idx->getPole());
        const double eps = 1.0e-10;
        int j = idx->getPole() == SOUTH_POLE ? 1 : mesh.getNumGrid(1, CENTER)-2;
        int k = (*idx)(2, CENTER);
        double sinLat = mesh.getSinLat(CENTER, j);
        double cosLat = mesh.getCosLat(CENTER, j);
        // horizontal velocity
        y[0] = 0.0; y[1] = 0.0;
        bool match = false;
        double ws = 0.0;
        for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
            double d = domain.calcDistance(x, mesh.getGridCoord(0, CENTER, i),
                                           sinLat, cosLat);
            if (d < eps) {
                y[0] = ring.getTransformedData(timeLevel, 0, i, k);
                y[1] = ring.getTransformedData(timeLevel, 1, i, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
                // cout << setw(5) << i << setw(20) << w << endl;
                ws += w;
                y[0] += w*ring.getTransformedData(timeLevel, 0, i, k);
                y[1] += w*ring.getTransformedData(timeLevel, 1, i, k);
            }
        }
        if (!match) {
            y[0] /= ws;
            y[1] /= ws;
        }
        // vertical velocity
        if (domain.getNumDim() == 3) {
            REPORT_ERROR("Under construction!");
            // linear interpolation
            int i = (*idx)(0, CENTER);
            double z1 = mesh.getGridCoord(2, EDGE, k);
            double z2 = mesh.getGridCoord(2, EDGE, k+1);
            double c = (z2-x(2))/(z2-z1);
            y(2) = c*f(timeLevel, 2, i, j, k)+(1.0-c)*f(timeLevel, 2, i, j, k+1);
        }
        if (!idx->isMoveOnPole()) {
            y.transformFromPS(x);
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            for (int m = 0; m < 2; ++m) {
                i1 = (*idx)(0, f.getStaggerType(m, 0));
                i2 = i1+1; i3 = i1; i4 = i2;
                j1 = (*idx)(1, f.getStaggerType(m, 1));
                j2 = j1; j3 = j1+1; j4 = j3;
                // cout << "----------------------------------" << endl;
                // cout << " i1 =" << setw(10) << i1 << endl;
                // cout << " i2 =" << setw(10) << i2 << endl;
                // cout << " j1 =" << setw(10) << j1 << endl;
                // cout << " j2 =" << setw(10) << j2 << endl;
                double x1 = mesh.getGridCoord(0, f.getStaggerType(m, 0), i1);
                double x2 = mesh.getGridCoord(0, f.getStaggerType(m, 0), i2);
                double X = (x(0)-x1)/(x2-x1);
                double y1 = mesh.getGridCoord(1, f.getStaggerType(m, 1), j1);
                double y2 = mesh.getGridCoord(1, f.getStaggerType(m, 1), j3);
                double Y = (x(1)-y1)/(y2-y1);
                // cout << "----------------------------------" << endl;
                // cout << " x1 =" << setw(10) << x1 << endl;
                // cout << " x2 =" << setw(10) << x2 << endl;
                // cout << " y1 =" << setw(10) << y1 << endl;
                // cout << " y2 =" << setw(10) << y2 << endl;
                // cout << "----------------------------------" << endl;
                // cout << " X  =" << setw(10) << X << endl;
                // cout << " Y  =" << setw(10) << Y << endl;
                double f1 = f(timeLevel, m, i1, j1, 0);
                double f2 = f(timeLevel, m, i2, j2, 0);
                double f3 = f(timeLevel, m, i3, j3, 0);
                double f4 = f(timeLevel, m, i4, j4, 0);
                // cout << "----------------------------------" << endl;
                // cout << " f1 =" << setw(10) << f1 << endl;
                // cout << " f2 =" << setw(10) << f2 << endl;
                // cout << " f3 =" << setw(10) << f3 << endl;
                // cout << " f4 =" << setw(10) << f4 << endl;
                double a = f1;
                double b = f2-f1;
                double c = f3-f1;
                double d = f1-f2-f3+f4;
                // cout << "----------------------------------" << endl;
                // cout << " a  =" << setw(10) << a << endl;
                // cout << " b  =" << setw(10) << b << endl;
                // cout << " c  =" << setw(10) << c << endl;
                // cout << " d  =" << setw(10) << d << endl;
                y(m) = a+b*X+c*Y+d*X*Y;
                // cout << "----------------------------------" << endl;
                // cout << " y(" << m << ") =" << setw(10) << y(m) << endl;
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
        if (idx->isMoveOnPole()) {
            y.transformToPS(x);
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const RLLTensorField &f,
                    const SpaceCoord &x, mat &y, RLLMeshIndex *idx_) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    const SphereDomain &domain = static_cast<const SphereDomain&>(mesh.getDomain());
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh.getDomain().getNumDim());
        idx->locate(mesh, x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    // cout << " x(0) =" << setw(10) << x(0) << endl;
    // cout << " x(1) =" << setw(10) << x(1) << endl;
    // idx->print();
    if (idx->isInPolarCap()) {
        const double eps = 1.0e-10;
        int j = idx->getPole() == SOUTH_POLE ? 1 : mesh.getNumGrid(1, CENTER)-2;
        int k = (*idx)(2, CENTER);
        double sinLat = mesh.getSinLat(CENTER, j);
        double cosLat = mesh.getCosLat(CENTER, j);
        y.zeros();
        bool match = false;
        double ws = 0.0;
        for (int i = 0; i < mesh.getNumGrid(0, CENTER); ++i) {
            double d = domain.calcDistance(x, mesh.getGridCoord(0, CENTER, i),
                                           sinLat, cosLat);
            if (d < eps) {
                for (int m1 = 0; m1 < domain.getNumDim(); ++m1) {
                    for (int m2 = 0; m2 < domain.getNumDim(); ++m2) {
                        y(m1, m2) = f(timeLevel, m1, m2, i, j, k);
                    }
                }
                match = true;
                break;
            } else {
                double w = 1.0/d;
                // cout << setw(5) << i << setw(20) << w << endl;
                ws += w;
                for (int m1 = 0; m1 < domain.getNumDim(); ++m1) {
                    for (int m2 = 0; m2 < domain.getNumDim(); ++m2) {
                        y(m1, m2) += w*f(timeLevel, m1, m2, i, j, k);
                    }
                }
            }
        }
        if (!match) {
            y /= ws;
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, CENTER);
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, CENTER);
            j2 = j1; j3 = j1+1; j4 = j3;
//            cout << "----------------------------------" << endl;
//            cout << " i1 =" << setw(10) << i1 << endl;
//            cout << " i2 =" << setw(10) << i2 << endl;
//            cout << " j1 =" << setw(10) << j1 << endl;
//            cout << " j2 =" << setw(10) << j2 << endl;
            double x1 = mesh.getGridCoord(0, CENTER, i1);
            double x2 = mesh.getGridCoord(0, CENTER, i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh.getGridCoord(1, CENTER, j1);
            double y2 = mesh.getGridCoord(1, CENTER, j3);
            double Y = (x(1)-y1)/(y2-y1);
            assert(X >= 0.0 && X <= 1.0 && Y >= 0.0 && Y <= 1.0);
//            cout << "----------------------------------" << endl;
//            cout << " x1 =" << setw(10) << x1 << endl;
//            cout << " x2 =" << setw(10) << x2 << endl;
//            cout << " y1 =" << setw(10) << y1 << endl;
//            cout << " y2 =" << setw(10) << y2 << endl;
//            cout << "----------------------------------" << endl;
//            cout << " X  =" << setw(10) << X << endl;
//            cout << " Y  =" << setw(10) << Y << endl;
            for (int m1 = 0; m1 < domain.getNumDim(); ++m1) {
                for (int m2 = 0; m2 < domain.getNumDim(); ++m2) {
                    double f1 = f(timeLevel, m1, m2, i1, j1, 0);
                    double f2 = f(timeLevel, m1, m2, i2, j2, 0);
                    double f3 = f(timeLevel, m1, m2, i3, j3, 0);
                    double f4 = f(timeLevel, m1, m2, i4, j4, 0);
//                    cout << "----------------------------------" << endl;
//                    cout << " f1 =" << setw(10) << f1 << endl;
//                    cout << " f2 =" << setw(10) << f2 << endl;
//                    cout << " f3 =" << setw(10) << f3 << endl;
//                    cout << " f4 =" << setw(10) << f4 << endl;
                    double a = f1;
                    double b = f2-f1;
                    double c = f3-f1;
                    double d = f1-f2-f3+f4;
//                    cout << "----------------------------------" << endl;
//                    cout << " a  =" << setw(10) << a << endl;
//                    cout << " b  =" << setw(10) << b << endl;
//                    cout << " c  =" << setw(10) << c << endl;
//                    cout << " d  =" << setw(10) << d << endl;
                    y(m1, m2) = a+b*X+c*Y+d*X*Y;
//                    cout << "----------------------------------" << endl;
//                    cout << " y(" << m1 << "," << m2 << ") =" << setw(10) << y(m1, m2) << endl;
                }
            }
        } else {
            REPORT_ERROR("Under construction!");
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

}
