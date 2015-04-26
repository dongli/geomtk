#ifndef __GEOMTK_TimeLevelIndex__
#define __GEOMTK_TimeLevelIndex__

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
    int get() const { return idx; }

    /**
     *  Check if the index points to the current time level.
     *
     *  @return The boolean flag.
     */
    bool isCurrentIndex() const { return idx == currFullIdx; }

    /**
     *  Offset the relative index by full levels.
     *
     *  @param offset the full levels to offset.
     *
     *  @return The full relative time level index.
     */
    TimeLevelIndex operator+(int offset) const;
    TimeLevelIndex operator-(int offset) const;

    /**
     *  Offset the relative index by half levels.
     *
     *  @param offset the half levels to offset.
     *
     *  @return The half relative time level index.
     */
    TimeLevelIndex operator+(double offset) const;
    TimeLevelIndex operator-(double offset) const;

    /**
     *  Reset the indices.
     */
    void reset();

    /**
     *  Shift the indices cyclically.
     */
    void shift();
};

} // geomtk

#include "TimeLevelIndex-impl.h"

#endif // __GEOMTK_TimeLevelIndex__
