#ifndef __Geomtk_TimeLevels__
#define __Geomtk_TimeLevels__

#include "geomtk_commons.h"
#include "TimeLevelIndex.h"

namespace geomtk {

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
    inline int numLevel(bool includeHalfLevel = false);

    /**
     *  Get the variable on the given time level (relative index).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The variable.
     */
    const T& level(const TimeLevelIndex<N> &timeIdx) const {
        return data[timeIdx.get()];
    }

    /**
     *  Get the variable on the given time level (relative index).
     *
     *  @param timeIdx the time level index.
     *
     *  @return The variable.
     */
    T& level(const TimeLevelIndex<N> &timeIdx) {
        return data[timeIdx.get()];
    }

    /**
     *  Get the variable on the given time level (absolute index).
     *
     *  @param i the time level index.
     *
     *  @return The variable.
     */
    const T& level(int i) const {
        return data[i];
    }

    /**
     *  Get the variable on the given time level (absolute index).
     *
     *  @param i the time level index.
     *
     *  @return The variable.
     */
    T& level(int i) {
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

} // geomtk

#include "TimeLevels-impl.h"

#endif // __Geomtk_TimeLevels__
