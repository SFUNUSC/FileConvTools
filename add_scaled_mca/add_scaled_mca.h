#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define S32K   32768
#define NSPECT 10

//PARAMETERS

char outName[256];

float inpHist1[NSPECT][S32K],inpHist2[NSPECT][S32K];
float outHist[NSPECT][S32K];
int mcaHist[NSPECT][S32K];

