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
    // idx->print();
    // cout << setw(5) << mesh.getNumGrid(0, CENTER) << setw(5) << mesh.getNumGrid(0, EDGE) << endl;
    // cout << setw(5) << mesh.getNumGrid(1, CENTER) << setw(5) << mesh.getNumGrid(1, EDGE) << endl;
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
                double x1 = mesh.getGridCoord(0, g->getStaggerType(m, 0), i1);
                double x2 = mesh.getGridCoord(0, g->getStaggerType(m, 0), i2);
                double X = (x(0)-x1)/(x2-x1);
                double y1 = mesh.getGridCoord(1, g->getStaggerType(m, 1), j1);
                double y2 = mesh.getGridCoord(1, g->getStaggerType(m, 1), j2);
                double Y = (x(0)-y1)/(y2-y1);
                double f1 = (*g)(timeLevel, m, i1, j1, 0);
                double f2 = (*g)(timeLevel, m, i2, j2, 0);
                double f3 = (*g)(timeLevel, m, i3, j3, 0);
                double f4 = (*g)(timeLevel, m, i4, j4, 0);
                double a = f1;
                double b = f2-f1;
                double c = f3-f1;
                double d = f1-f2-f3+f4;
                y(m) = a+b*X+c*Y+d*X*Y;
            }
            break;
        case TRILINEAR:

            break;
    }
    if (idx_ == NULL) {
        delete idx;
    }
}
