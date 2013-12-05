#include "StructuredRegrid.h"
#include "StructuredMeshIndex.h"
#include "StructuredScalarField.h"
#include "StructuredVectorField.h"

StructuredRegrid::StructuredRegrid(Mesh &mesh) : Regrid(mesh) {
}

StructuredRegrid::~StructuredRegrid() {
}

void StructuredRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                           const SpaceCoord &x, double &y, MeshIndex *idx_) {
    REPORT_ERROR("Under construction!");
    const StructuredScalarField *g = dynamic_cast<const StructuredScalarField*>(&f);
    if (g == NULL) {
        REPORT_ERROR("Mesh should be compatible with StructuredScalarField!");
    }
}

void StructuredRegrid::run(RegridMethod method, int timeLevel, const Field &f,
                           const SpaceCoord &x, vec &y, MeshIndex *idx_) {
    const StructuredVectorField *g = dynamic_cast<const StructuredVectorField*>(&f);
    if (g == NULL) {
        REPORT_ERROR("Mesh should be compatible with StructuredVectorField!");
    }
    StructuredMesh &mesh = static_cast<StructuredMesh&>(*(this->mesh));
    StructuredMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new StructuredMeshIndex(mesh);
        idx->locate(x);
    } else {
        idx = static_cast<StructuredMeshIndex*>(idx_);
    }
    // cout << " x(0) =" << setw(10) << x(0) << endl;
    // cout << " x(1) =" << setw(10) << x(1) << endl;
    // idx->print();
    switch (method) {
        case BILINEAR:
            int i1, i2, i3, i4, j1, j2, j3, j4;
            for (int m = 0; m < 2; ++m) {
                i1 = (*idx)(0, g->getStaggerType(m, 0));
                i2 = i1+1; i3 = i1; i4 = i2;
                j1 = (*idx)(1, g->getStaggerType(m, 1));
                j2 = j1; j3 = j1+1; j4 = j3;
                if (g->getStaggerType(1, 1) == EDGE &&
                    (j1 == -1 || j3 == mesh.getNumGrid(1, EDGE))) {
                    REPORT_ERROR("Vector field component 1 is one CENTER-EDGE mesh, "
                                 "and the point is located outside its region!");
                }
                // cout << "----------------------------------" << endl;
                // cout << " i1 =" << setw(10) << i1 << endl;
                // cout << " i2 =" << setw(10) << i2 << endl;
                // cout << " j1 =" << setw(10) << j1 << endl;
                // cout << " j2 =" << setw(10) << j2 << endl;
                double x1 = mesh.getGridCoord(0, g->getStaggerType(m, 0), i1);
                double x2 = mesh.getGridCoord(0, g->getStaggerType(m, 0), i2);
                double X = (x(0)-x1)/(x2-x1);
                double y1 = mesh.getGridCoord(1, g->getStaggerType(m, 1), j1);
                double y2 = mesh.getGridCoord(1, g->getStaggerType(m, 1), j3);
                double Y = (x(1)-y1)/(y2-y1);
                // cout << "----------------------------------" << endl;
                // cout << " x1 =" << setw(10) << x1 << endl;
                // cout << " x2 =" << setw(10) << x2 << endl;
                // cout << " y1 =" << setw(10) << y1 << endl;
                // cout << " y2 =" << setw(10) << y2 << endl;
                // cout << "----------------------------------" << endl;
                // cout << " X  =" << setw(10) << X << endl;
                // cout << " Y  =" << setw(10) << Y << endl;
                double f1 = (*g)(timeLevel, m, i1, j1, 0);
                double f2 = (*g)(timeLevel, m, i2, j2, 0);
                double f3 = (*g)(timeLevel, m, i3, j3, 0);
                double f4 = (*g)(timeLevel, m, i4, j4, 0);
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
            break;
        case TRILINEAR:

            break;
    }
    if (idx_ == NULL) {
        delete idx;
    }
}
