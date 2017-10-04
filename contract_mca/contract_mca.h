#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstring>

#define S32K   32768
#define NSPECT 100

//PARAMETERS

//file I/O
char outName[256];

float inpHist[NSPECT][S32K];
float outHist[NSPECT][S32K];
int mcaHist[NSPECT][S32K];

//forward declarations
void readFileToArrays(const char *);



