#ifndef __TimeLevels__
#define __TimeLevels__

/**
 * TimeLevels.
 * A template class for making variables multi-time-levels.
 * @tparam T the variable type.
 * @tparam N the number of time levels that are stored.
 */

namespace geomtk {

template <typename T, int N>
class TimeLevels {
    T data[N];

public:
    /**
     * Return the number of time levels.
     */
    int getNumLevel() { return N; }

    /**
     * Get the variable on the needed time level.
     */
    T get(int level) const;

    /**
     * Get the variable on the needed time level.
     */
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

}

#endif /* defined(__Lady__TimeLevels__) */
