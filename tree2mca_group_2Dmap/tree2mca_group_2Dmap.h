//ROOT stuff
#include "TRandom3.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TFile.h"
#include "TApplication.h"
#include "TH1.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define S32K   32768
#define NSPECT 10

//forward declarations
void addTreeDataToOutHist();
void readGroupMap();
double FWHM_response(double);

// File I/O
FILE *config,*customFile;
char cfgstr[256],str1[256],str2[256];

// tree2mca_group parameters
char sort_path[256],weight_path[256],gate_path[256],inp_filename[256],sort_tree_name[256],gate_tree_name[256],out_filename[256],file_handler[256];
char group_file[256],pos_path[256],col_path[256],csi_path[256]; // group sorting stuff
double custom_gates[NSPECT][4];
double gate_weight[NSPECT];
int num_custom_gates=0;
double sort_scaling=1.0;//data scaling factor
double sort_shift=0.0;//data shift in bins
double gate_scaling=1.0;//gate scaling factor
double gate_shift=0.0;//gate shift in bins
bool listMode=false; //whether to sort tree files from a list 
bool fwhmResponse=false; //whether to do energy convolution
double fwhmF,fwhmG,fwhmH,fwhmTauH,fwhmTauL,wH,wL,wG; //energy convolution parameters
bool output_specified=false;
bool file_handler_specified=false;
int use_custom_gates=0;//0=no gates,1=1D gates,2=2D gates, etc.
bool use_gate_weights=false;
bool use_weights=false;

//binnedavgtxt parameters
char x_branch[256],y_branch[256];
int x_leaf, y_leaf;
double val_scaling=1.0;
double binSize=1.0;
double max_x;
bool use_max_x=false;

int outHist[NSPECT][S32K];

// for sorting
TTree *stree,*gtree;
TLeaf *sortLeaf, *weightLeaf, *gateLeaf;
TBranch *sortBranch, *weightBranch, *gateBranch;
TApplication *theApp;
TRandom3 *randGen;

// group stuff
int  group_map[17][5][129][129];
TLeaf *posLeaf,*colLeaf,*csiLeaf,*csiELeaf;
TBranch *posBranch,*colBranch,*csiBranch,*csiEBranch;
// group hist
int groupHist[NSPECT][S32K];
