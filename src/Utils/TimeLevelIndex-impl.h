namespace geomtk {

template <int N>
int TimeLevelIndex<N>::currFullIdx = 0;
template <int N>
int TimeLevelIndex<N>::currHalfIdx = N;

template <int N>
TimeLevelIndex<N>::TimeLevelIndex() {
    idx = currFullIdx;
}

template <int N>
TimeLevelIndex<N> TimeLevelIndex<N>::operator+(int offset) const {
    int level = idx-currFullIdx;
    if (level < 0) {
        level += N;
    }
#ifndef NDEBUG
    if ((offset < 0 && -offset > level) ||
        (offset > 0 &&  offset > N-1-level)) {
        REPORT_ERROR("Argument 'offset' (" << offset << ") is out of range!");
    }
#endif
    TimeLevelIndex res;
    res.idx = idx+offset;
    if (res.idx >= N) {
        res.idx -= N;
    } else if (res.idx < 0) {
        res.idx += N;
    }
    return res;
}

template <int N>
TimeLevelIndex<N> TimeLevelIndex<N>::operator-(int offset) const {
    int level = idx-currFullIdx;
    if (level < 0) {
        level += N;
    }
#ifndef NDEBUG
    if ((offset < 0 && -offset > N-1-level) ||
        (offset > 0 &&  offset > level)) {
        REPORT_ERROR("Argument 'offset' (" << offset << ") is out of range!");
    }
#endif
    TimeLevelIndex res;
    res.idx = idx-offset;
    if (res.idx >= N) {
        res.idx -= N;
    } else if (res.idx < 0) {
        res.idx += N;
    }
    return res;
}

template <int N>
TimeLevelIndex<N> TimeLevelIndex<N>::operator+(double offset) const {
#ifndef NDEBUG
    if (offset/0.5-int(offset/0.5) != 0.0) {
        REPORT_ERROR("Argument 'offset' (" << offset <<
                     ") should be a multiple of 0.5!");
    }
#endif
    int level = idx-currFullIdx;
    if (level < 0) {
        level += N;
    }
    int half_offset = int(offset-0.5);
#ifndef NDEBUG
    if ((half_offset < 0 && -half_offset > level-1) ||
        (half_offset > 0 &&  half_offset > N-2-level)) {
        REPORT_ERROR("Argument 'offset' (" << offset << ") is out of range!");
    }
#endif
    TimeLevelIndex res;
    res.idx = currHalfIdx+level+half_offset;
    if (res.idx == 2*N-1) {
        res.idx = N;
    }
    return res;
}

template <int N>
TimeLevelIndex<N> TimeLevelIndex<N>::operator-(double offset) const {
#ifndef NDEBUG
    if (offset/0.5-int(offset/0.5) != 0.0) {
        REPORT_ERROR("Argument 'offset' (" << offset <<
                     ") should be a multiple of 0.5!");
    }
#endif
    int level = idx-currFullIdx;
    if (level < 0) {
        level += N;
    }
    int half_offset = int(offset+0.5);
#ifndef NDEBUG
    if ((half_offset < 0 && -half_offset > N-2-level) ||
        (half_offset > 0 &&  half_offset > level)) {
        REPORT_ERROR("Argument 'offset' (" << offset << ") is out of range!");
    }
#endif
    TimeLevelIndex res;
    res.idx = currHalfIdx+level-half_offset;
    if (res.idx == 2*N-1) {
        res.idx = N;
    }
    return res;
}

template <int N>
void TimeLevelIndex<N>::reset() {
    currFullIdx = 0;
    currHalfIdx = N;
    idx = currFullIdx;
}
    
template <int N>
void TimeLevelIndex<N>::shift() {
    currFullIdx++;
    if (currFullIdx == N) {
        currFullIdx = 0;
    }
    currHalfIdx++;
    if (currHalfIdx == 2*N-1) {
        currHalfIdx = N;
    }
    idx++;
    if (idx == N) {
        idx = 0;
    }
}

} // geomtk