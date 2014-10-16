namespace geomtk {

template <typename T, int N>
void RLLRegrid::run(RegridMethod method, const TimeLevelIndex<N> &timeIdx,
                    const RLLField<T, N> &f, const SphereCoord &x, T &y,
                    RLLMeshIndex *idx_) {
    RLLMeshIndex *idx;
    if (idx_ == NULL) {
        idx = new RLLMeshIndex(mesh().domain().numDim());
        idx->locate(mesh(), x);
    } else {
        idx = idx_;
    }
    if (idx->isInPolarCap()) {
        // use inverse distance weighting interpolation
        assert(f.staggerLocation() == RLLStagger::Location::CENTER);
        const SphereDomain &domain = mesh().domain();
        const double eps = 1.0e-10;
        int j = idx->pole() == SOUTH_POLE ? 1 : mesh().numGrid(1, RLLStagger::GridType::FULL)-2;
        int k = (*idx)(2, RLLStagger::GridType::FULL);
        double sinLat = mesh().sinLat(RLLStagger::GridType::FULL, j);
        double cosLat = mesh().cosLat(RLLStagger::GridType::FULL, j);
        y = 0.0;
        bool match = false;
        double ws = 0.0;
        for (int i = 0; i < mesh().numGrid(0, RLLStagger::GridType::FULL); ++i) {
            // TODO: Use mesh().gridCoord(...) instead.
            double d = domain.calcDistance(x, mesh().gridCoordComp(0, RLLStagger::GridType::FULL, i),
                                           sinLat, cosLat);
            if (d < eps) {
                y = f(timeIdx, i, j, k);
                match = true;
                break;
            } else {
                double w = 1.0/d;
                ws += w;
                y += w*f(timeIdx, i, j, k);
            }
        }
        if (!match) {
            y /= ws;
        }
    } else {
        if (method == BILINEAR) {
            int i1, i2, i3, i4, j1, j2, j3, j4;
            i1 = (*idx)(0, f.gridType(0));
            i2 = i1+1; i3 = i1; i4 = i2;
            j1 = (*idx)(1, f.gridType(1));
            j2 = j1; j3 = j1+1; j4 = j3;
            if (f.gridType(1) == GridType::HALF &&
                (j1 == -1 || j3 == mesh().numGrid(1, GridType::HALF))) {
                REPORT_ERROR("Check this error!");
            }
            double x1 = mesh().gridCoordComp(0, f.gridType(0), i1);
            double x2 = mesh().gridCoordComp(0, f.gridType(0), i2);
            double X = (x(0)-x1)/(x2-x1);
            double y1 = mesh().gridCoordComp(1, f.gridType(1), j1);
            double y2 = mesh().gridCoordComp(1, f.gridType(1), j3);
            double Y = (x(1)-y1)/(y2-y1);
#ifndef NDEBUG
            assert(X >= 0.0 && X <= 1.0 && Y >= 0.0 && Y <= 1.0);
#endif
            double f1 = f(timeIdx, i1, j1);
            double f2 = f(timeIdx, i2, j2);
            double f3 = f(timeIdx, i3, j3);
            double f4 = f(timeIdx, i4, j4);
            double a = f1;
            double b = f2-f1;
            double c = f3-f1;
            double d = f1-f2-f3+f4;
            y = a+b*X+c*Y+d*X*Y;
        } else if (method == TRILINEAR) {
            REPORT_ERROR("Under construction!");
        }
    }
    if (idx_ == NULL) {
        delete idx;
    }
}

} // geomtk
