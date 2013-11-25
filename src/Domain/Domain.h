#ifndef __Domain__
#define __Domain__

#include "commons.h"

enum BndType {
    PERIODIC, OPEN, POLE, RIGID
};

class Domain {
protected:
    int numDim;
    vec axisStarts;
    vec axisEnds;
    vec axisSpans;
    BndType *bndTypeStarts;
    BndType *bndTypeEnds;

public:
    Domain(int numDim);
    virtual ~Domain();

    virtual void setAxis(int dim,
                         double start, BndType bndTypeStart,
                         double end, BndType bndTypeEnd);
    virtual double getAxisStart(int dim);
    virtual double getAxisEnd(int dim);
    virtual double getAxisSpan(int dim);
    virtual BndType getAxisStartBndType(int dim);
    virtual BndType getAxisEndBndType(int dim);
};

#endif
