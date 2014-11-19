namespace geomtk {

template <class MeshType>
template <class FieldType, int N>
void StructuredFilter<MeshType>::
run(const TimeLevelIndex<N> &timeIdx, FieldType &field) {
    switch (this->scheme) {
    case NINE_POINT_SMOOTHING:
        runNinePointSmoothing(timeIdx, field);
        break;
    }
}

template <class MeshType>
template <class FieldType, int N>
void StructuredFilter<MeshType>::
runNinePointSmoothing(const TimeLevelIndex<N> &timeIdx, FieldType &field) {
    assert(field.staggerLocation() == Location::CENTER);
    assert(this->mesh->domain().axisStartBndType(0) == PERIODIC);
    assert(this->mesh->domain().numDim() == 2);
    const double p = 0.5;
    const double q = 0.25;
    double tmp[this->mesh->totalNumGrid(field.staggerLocation())];
    int l = 0;
    for (int j = this->mesh->js(GridType::FULL)+1; j < this->mesh->je(GridType::FULL); ++j) {
        for (int i = this->mesh->is(GridType::FULL); i <= this->mesh->ie(GridType::FULL); ++i) {
            double f0 = field(timeIdx, i,   j  );
            double f1 = field(timeIdx, i-1, j-1);
            double f2 = field(timeIdx, i-1, j  );
            double f3 = field(timeIdx, i-1, j+1);
            double f4 = field(timeIdx, i,   j+1);
            double f5 = field(timeIdx, i+1, j+1);
            double f6 = field(timeIdx, i+1, j  );
            double f7 = field(timeIdx, i+1, j-1);
            double f8 = field(timeIdx, i,   j-1);
            tmp[l++] = f0+p*0.25*(f2+f4+f6+f8-4*f0)+q*0.25*(f1+f3+f5+f7-4*f0);
        }
    }
    l = 0;
    for (int j = this->mesh->js(GridType::FULL)+1; j < this->mesh->je(GridType::FULL); ++j) {
        for (int i = this->mesh->is(GridType::FULL); i <= this->mesh->ie(GridType::FULL); ++i) {
            field(timeIdx, i, j) = tmp[l++];
        }
    }
}

} // geomtk
