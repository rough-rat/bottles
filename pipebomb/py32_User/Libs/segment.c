
#include "segment.h"

bool isSegmentActiveForNumber(int segmentNum, int number){
    switch (number){
        case 0: return segmentNum != 3;
        case 1: return segmentNum == 5 || segmentNum == 2;
        case 2: return segmentNum != 1 && segmentNum != 5;
        case 3: return segmentNum != 1 && segmentNum != 4;
        case 4: return segmentNum == 1 || segmentNum == 2 || segmentNum == 3 || segmentNum == 5;
        case 5: return segmentNum != 2 && segmentNum != 4;
        case 6: return segmentNum != 2;
        case 7: return segmentNum == 0 || segmentNum == 2 || segmentNum == 5;
        case 8: return true;
        case 9: return segmentNum != 4;
    }
    return false;
}

void drawSevenSegment(charlie_t* charlie, segment_t* segment){
    // charlie->matrixBuf[segment->segmentNumber[i]] = 1;
    printf(" %c \n", charlie->matrixBuf[segment->segmentNumber[0]] ? '=' : ' ');

    printf("%c %c\n",
        charlie->matrixBuf[segment->segmentNumber[1]] ? '|' : ' ',
        charlie->matrixBuf[segment->segmentNumber[2]] ? '|' : ' '
    );
    printf(" %c \n", charlie->matrixBuf[segment->segmentNumber[3]] ? '=' : ' ');

    printf("%c %c\n",
        charlie->matrixBuf[segment->segmentNumber[4]] ? '|' : ' ',
        charlie->matrixBuf[segment->segmentNumber[5]] ? '|' : ' '
    );
    printf(" %c \n", charlie->matrixBuf[segment->segmentNumber[6]] ? '=' : ' ');
}

void setSevenSegment(charlie_t* charlie, segment_t* segment, int value){
    for (int i = 0; i < 7; i++){
        charlie->matrixBuf[segment->segmentNumber[i]] = isSegmentActiveForNumber(i, value);
        // charlieSetPixel(charlie, 0, segment->segmentNumber[i], isSegmentActiveForNumber(i, value));
    }
}