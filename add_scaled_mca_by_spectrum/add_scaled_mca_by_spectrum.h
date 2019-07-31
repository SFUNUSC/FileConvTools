#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define S32K   32768
#define NSPECT 10

//PARAMETERS

char outName[256];

float inpHist1[NSPECT][S32K],inpHist2[NSPECT][S32K];
double scale1[NSPECT],scale2[NSPECT];
float outHist[NSPECT][S32K];
int mcaHist[NSPECT][S32K];
char str[256];

