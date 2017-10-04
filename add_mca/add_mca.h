#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstring>

#define S32K   32768
#define NSPECT 100

//PARAMETERS
float outHist[NSPECT][S32K];
int mcaHist[NSPECT][S32K];;
float inpHist1[NSPECT][S32K],inpHist2[NSPECT][S32K];
char str[256], outName[256];

//forward declarations
void readFileToArrays(const char *);
