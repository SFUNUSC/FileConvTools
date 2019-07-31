//ROOT stuff
#include "TRandom3.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TFile.h"
#include "TApplication.h"
#include "TH1.h"
#include "TH2.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define S32K   32768
#define NSPECT 10

//forward declarations
void addTreeDataToOutHist();
void writeGroupMap();
double FWHM_response(double);

// File I/O
FILE *config,*customFile;
char cfgstr[256],str1[256],str2[256];

// tree2doppler_2Dmap parameters
char sort_path[256],weight_path[256],inp_filename[256],sort_tree_name[256],out_filename[256];
char group_file[256],pos_path[256],col_path[256],csi_path[256]; // group sorting stuff
bool listMode=false; //whether to sort tree files from a list
bool output_specified=false;
bool use_weights=false;

int outHist[NSPECT][S32K];
float dOutHist[NSPECT][S32K];

// for sorting
TTree *stree,*gtree;
TLeaf *sortLeaf, *weightLeaf;
TBranch *sortBranch, *weightBranch;
TApplication *theApp;
TRandom3 *randGen;

// group stuff
int  group_map[17][5][129][129];
TLeaf *posLeaf,*colLeaf,*csiLeaf,*csiELeaf;
TBranch *posBranch,*colBranch,*csiBranch,*csiEBranch;
// group hist
int groupHist[NSPECT][S32K];
double dg1,dg2,dg3,dg4,dg5;

// doppler map stuff
double dopplerFactor[17][4][129][129];
double counter[17][4][129][129];
TH2D *d;
TH1D *d1;

