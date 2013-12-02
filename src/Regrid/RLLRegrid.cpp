#include "RLLRegrid.h"
#include "RLLMesh.h"
#include "RLLMeshIndex.h"
#include "RLLScalarField.h"
#include "RLLVectorField.h"
#include "RLLVelocityField.h"

RLLRegrid::RLLRegrid(Mesh &mesh) : Regrid(mesh) {
}

RLLRegrid::~RLLRegrid() {
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                    const SpaceCoord &x, double &y, MeshIndex *idx_) {
    const RLLScalarField *g = dynamic_cast<const RLLScalarField*>(&f);
    if (g == NULL) {
        REPORT_ERROR("Mesh should comply RLLScalarField!");
    }
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(*mesh);
        idx->locate(x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation

    } else {
        switch (method) {
            case BILINEAR:

                break;
            case TRILINEAR:
                REPORT_ERROR("Under construction!");
                break;
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                    const SpaceCoord &x, vec &y, MeshIndex *idx_) {
    const RLLVectorField *g = dynamic_cast<const RLLVectorField*>(&f);
    if (g == NULL) {
        REPORT_ERROR("Mesh should comply RLLVectorField!");
    }
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(*mesh);
        idx->locate(x);
    } else {
        idx = static_cast<RLLMeshIndex*>(idx_);
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation
        
    } else {

    }
    if (idx_ == NULL) {
        delete idx;
    }
}

void RLLRegrid::run(RegridMethod method, int timeLevel, const RLLVelocityField &f,
                    const SpaceCoord &x, SphereVelocity &y) {
    const RLLMesh &mesh = static_cast<const RLLMesh&>(*(this->mesh));
    RLLMeshIndex idx(*(this->mesh));
    idx.locate(x);
    // idx.print();
    if (idx.isInPolarCap()) {
        const SphereDomain &domain = static_cast<const SphereDomain&>(mesh.getDomain());
        const PolarRing &ring = f.getPolarRing(idx.getPole());
        const double eps = 1.0e-10;
        int j = idx.getPole() == SOUTH_POLE ? 1 : mesh.getNumGrid(1, CENTER)-2;
        int k = idx(2, CENTER);
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
                y(0) = ring.getOriginalData(timeLevel, 0, i, k);
                y(1) = ring.getOriginalData(timeLevel, 1, i, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
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
        }
    } else {
        switch (method) {
            case BILINEAR:
                int i1, i2, i3, i4, j1, j2, j3, j4;
                for (int m = 0; m < 2; ++m) {
                    i1 = idx(0, f.getStaggerType(m, 0));
                    i2 = i1+1; i3 = i1; i4 = i2;
                    j1 = idx(1, f.getStaggerType(m, 1));
                    j2 = j1; j3 = j1+1; j4 = j3;
                    // cout << mesh.getNumGrid(0, CENTER, true) << endl;
                    // cout << mesh.getNumGrid(0, EDGE, true) << endl;
                    // cout << setw(5) << i1 << setw(5) << i2 << setw(5) << i3 << setw(5) << i4 << endl;
                    // cout << setw(5) << j1 << setw(5) << j2 << setw(5) << j3 << setw(5) << j4 << endl;
                    double x1 = mesh.getGridCoord(0, f.getStaggerType(m, 0), i1);
                    double x2 = mesh.getGridCoord(0, f.getStaggerType(m, 0), i2);
                    double X = (x(0)-x1)/(x2-x1);
                    double y1 = mesh.getGridCoord(1, f.getStaggerType(m, 1), j1);
                    double y2 = mesh.getGridCoord(1, f.getStaggerType(m, 1), j2);
                    double Y = (x(0)-y1)/(y2-y1);
                    double f1 = f(timeLevel, m, i1, j1, 0);
                    double f2 = f(timeLevel, m, i2, j2, 0);
                    double f3 = f(timeLevel, m, i3, j3, 0);
                    double f4 = f(timeLevel, m, i4, j4, 0);
                    double a = f1;
                    double b = f2-f1;
                    double c = f3-f1;
                    double d = f1-f2-f3+f4;
                    y(m) = a+b*X+c*Y+d*X*Y;
                }
                break;
            case TRILINEAR:
                REPORT_ERROR("Under construction!");
                break;
        }
    }
}
