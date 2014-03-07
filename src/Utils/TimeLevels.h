#ifndef __Geomtk_TimeLevels__
#define __Geomtk_TimeLevels__

#include "geomtk_commons.h"

namespace geomtk {

/**
 *  This class specifies the relative index of time levels, such as n, n+1, and
 *  n+1/2, so half time level is supported. The static 'shift' can be used to
 *  shift the indices cyclically.
 */
template <int N>
class TimeLevelIndex {
protected:
    static int currFullIdx;
    static int currHalfIdx;
    int idx;
public:
    TimeLevelIndex();

    /**
     *  Get the absolute time level index (can be half level).
     *
     *  @return The absolute time level index.
     */
    inline int get() const;

    /**
     *  Check if the index points to the current time level.
     *
     *  @return The boolean flag.
     */
    inline bool isCurrentIndex() const;

    /**
     *  Offset the relative index by full levels.
     *
     *  @param offset the full levels to offset.
     *
     *  @return The full relative time level index.
     */
    inline TimeLevelIndex operator+(int offset) const;
    inline TimeLevelIndex operator-(int offset) const;

    /**
     *  Offset the relative index by half levels.
     *
     *  @param offset the half levels to offset.
     *
     *  @return The half relative time level index.
     */
    inline TimeLevelIndex operator+(double offset) const;
    inline TimeLevelIndex operator-(double offset) const;

    /**
     *  Reset the indices.
     */
    inline void reset();

    /**
     *  Shift the indices cyclically.
     */
    inline void shift();
};

// =============================================================================

template <int N>
int TimeLevelIndex<N>::currFullIdx = 0;
template <int N>
int TimeLevelIndex<N>::currHalfIdx = N;

template <int N>
TimeLevelIndex<N>::TimeLevelIndex() {
    idx = currFullIdx;
}

template <int N>
int TimeLevelIndex<N>::get() const {
    return idx;
}
    
template <int N>
bool TimeLevelIndex<N>::isCurrentIndex() const {
    if (idx == currFullIdx) {
        return true;
    } else {
        return false;
    }
}

template <int N>
TimeLevelIndex<N> TimeLevelIndex<N>::operator+(int offset) const {
    int level = idx-currFullIdx;
    if (level < 0) {
        level += N;
    }
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
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
    
// -----------------------------------------------------------------------------
    
#define HAS_HALF_LEVEL true
#define INCLUDE_HALF_LEVEL true

/**
 *  A template class for making variables multi-time-levels.
 *
 *  @tparam T the variable type.
 *  @tparam N the number of time levels that are stored.
 */
template <typename T, int N>
class TimeLevels {
protected:
    T *data;
    bool halfLevel;
public:
    TimeLevels(bool hasHalfLevel = false);
    virtual ~TimeLevels();

    /**
     *  Get the time level number including half level if has.
     *
     *  @return The time level number.
     */
    inline int getNumLevel(bool includeHalfLevel = false);

    /**
     *  Get the variable on the given time level (relative index).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The variable.
     */
    const T& getLevel(const TimeLevelIndex<N> &timeIdx) const {
        return data[timeIdx.get()];
    }

    /**
     *  Get the variable on the given time level (relative index).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The variable.
     */
    T& getLevel(const TimeLevelIndex<N> &timeIdx) {
        return data[timeIdx.get()];
    }

    /**
     *  Get the variable on the given time level (absolute index).
     *
     *  @param i the time level index.
     *
     *  @return The variable.
     */
    const T& getLevel(int i) const {
        return data[i];
    }

    /**
     *  Get the variable on the given time level (absolute index).
     *
     *  @param i the time level index.
     *
     *  @return The variable.
     */
    T& getLevel(int i) {
        return data[i];
    }

    /**
     *  Return whether half levels are included.
     *
     *  @return The boolean flag.
     */
    inline bool hasHalfLevel() const;
    
    inline TimeLevels<T, N>& operator=(const TimeLevels<T, N> &other);
};

// =============================================================================

/*
    Without half levels
 
    Before shift:
     _______________________ 
    |       |       |       |
    |   n   |  n+1  |  n+2  |
    |_______|_______|_______|

    After shift:
     _______________________
    |       |       |       |
    |  n+2  |   n   |  n+1  |
    |_______|_______|_______|

    With half levels
 
    Before shift:
     _______________________________________ 
    |       |       |       |       |       |
    |   n   |  n+1  |  n+2  | n+1/2 | n+3/2 |
    |_______|_______|_______|_______|_______|

    After shift:
     _______________________________________ 
    |       |       |       |       |       |
    |  n+2  |   n   |  n+1  | n+3/2 | n+2/2 |
    |_______|_______|_______|_______|_______|
 */

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
int TimeLevels<T, N>::getNumLevel(bool includeHalfLevel) {
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

}

#endif
