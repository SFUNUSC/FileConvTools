#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define S32K   32768
#define NSPECT 100

//forward declarations
void readFileToArrays(const char *);

//PARAMETERS

//file I/O
FILE *config,*customFile;
char cfgstr[256],str1[256],str2[256];

//tree2mca parameters
char sort_path[256],gate_path[256],inp_filename[256],sort_tree_name[256],gate_tree_name[256],out_filename[256],file_handler[256];
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

//binnedavgtxt parameters
char x_branch[256],y_branch[256];
int x_leaf, y_leaf;
double val_scaling=1.0;
double binSize=1.0;
double max_x;
bool use_max_x=false;

int mcaHist[NSPECT][S32K];
int outHist[NSPECT][S32K];
