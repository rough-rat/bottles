

#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "charlie.h"

typedef struct {
    int segmentNumber[7];
} segment_t;

bool isSegmentActiveForNumber(int segmentNum, int number);

void drawSevenSegment(charlie_t* charlie, segment_t* segment);
void setSevenSegment(charlie_t* charlie, segment_t* segment, int value);


#endif // SEGMENT_H