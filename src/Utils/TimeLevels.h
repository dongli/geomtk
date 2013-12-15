#ifndef __TimeLevels__
#define __TimeLevels__

/**
 * TimeLevels.
 * A template class for making variables multi-time-levels.
 * @tparam T the variable type.
 * @tparam N the number of time levels that are stored.
 */

namespace geomtk {

#define HAS_HALF_LEVEL true

template <typename T, int N>
class TimeLevels {
protected:
    T *data;
    bool halfLevel;
public:
    TimeLevels(bool hasHalfLevel = false);
    virtual ~TimeLevels();

    /**
     * Return the number of time levels.
     */
    int getNumLevel();

    /**
     * Get the variable on the needed time level.
     */
    T getLevel(int level) const;

    /**
     * Get the variable on the needed time level.
     */
    T& getLevel(int level);

    bool hasHalfLevel() const;

    /**
     * Update the variable on half time level.
     * @note Assume two time levels.
     */
    void updateHalfLevel();
};

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
int TimeLevels<T, N>::getNumLevel() {
    if (halfLevel) {
        return N+(N-1);
    } else {
        return N;
    }
}

template <typename T, int N>
T TimeLevels<T, N>::getLevel(int level) const {
    return data[level];
}

template <typename T, int N>
T& TimeLevels<T, N>::getLevel(int level) {
    return data[level];
}

template <typename T, int N>
bool TimeLevels<T, N>::hasHalfLevel() const {
    return halfLevel;
}

template <typename T, int N>
void TimeLevels<T, N>::updateHalfLevel() {
    if (N != 2) {
        REPORT_ERROR("Half time level is only supported when there are two time levels!");
    }
    data[2] = (data[0]+data[1])*0.5;
}

}

#endif
