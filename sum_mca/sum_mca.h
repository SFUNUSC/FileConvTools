#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define S32K   32768
#define NSPECT 100

//PARAMETERS

//file I/O
FILE *config,*customFile;
char cfgstr[256],str1[256],str2[256];

int mcaHist[NSPECT][S32K];
float inpHist1[NSPECT][S32K],inpHist2[NSPECT][S32K];
float outHist[NSPECT][S32K];

char str[256],outName[256];
