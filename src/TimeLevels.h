#ifndef __TimeLevels__
#define __TimeLevels__

template <typename T, int N>
class TimeLevels {
    T data[N];

public:
    int getNumLevel() { return N; }
    T get(int level) const;
    T& get(int level);
};

template <typename T, int N>
T TimeLevels<T, N>::get(int level) const {
    return data[level];
}

template <typename T, int N>
T& TimeLevels<T, N>::get(int level) {
    return data[level];
}

#endif /* defined(__Lady__TimeLevels__) */
