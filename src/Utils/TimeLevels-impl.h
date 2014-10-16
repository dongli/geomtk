namespace geomtk {

template <typename T, int N>
TimeLevels<T, N>::TimeLevels(bool hasHalfLevel) {
    if (hasHalfLevel == HAS_HALF_LEVEL) {
        data = new T[N+(N-1)];
    } else {
        data = new T[N];
    }
    halfLevel = hasHalfLevel;
}

template <typename T, int N>
TimeLevels<T, N>::~TimeLevels() {
    delete [] data;
}

template <typename T, int N>
int TimeLevels<T, N>::numLevel(bool includeHalfLevel) {
    if (halfLevel && includeHalfLevel) {
        return N+(N-1);
    } else {
        return N;
    }
}

template <typename T, int N>
bool TimeLevels<T, N>::hasHalfLevel() const {
    return halfLevel;
}
    
template <typename T, int N>
TimeLevels<T, N>& TimeLevels<T, N>::operator=(const TimeLevels<T, N> &other) {
    if (this != &other) {
        if (halfLevel) {
            for (int i = 0; i < 2*N-1; ++i) {
                data[i] = other.data[i];
            }
        } else {
            for (int i = 0; i < N; ++i) {
                data[i] = other.data[i];
            }
        }
    }
    return *this;
}

} // geomtk