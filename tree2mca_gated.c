#include "tree2mca.h"
#include "read_config.c"

Int_t gate_data[NSPECT];
char str[256];

int main(int argc, char *argv[])
{

  //Creating an instance of TApplication
  //This is evidently needed for auto-loading of ROOT libraries, 
  //otherwise the program may crash upon execution depending on how ROOT
  //is set up.
  //http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14064
  int ac;
  char* av[10];
  theApp=new TApplication("App", &ac, av);

  same_branches = false;

  FILE *list, *output;
  TFile *inp;
  randGen = new TRandom();

  if(argc!=2)
    {
      printf("\ntree2mca_gated config_file\n");
      printf("\nTakes the data in the specified branch and leaf of the specified ROOT tree(s) and sorts it to .mca file(s) with spectra gated on the data in the specified gate branch and leaf.\n");
      printf("Eg. the sort data could refer to gamma ray energy, while the gate data could refer to detector number.  Output is then an .mca file containing gamma-ray spectra, where the spectrum number corresponds to the detector number.  The gate data should contain integer values.\n");
      printf("\nSORT_DATA_SCALING_FACTOR specifies a scaling factor for the sort data, which can be used to get different binning in the output spectra.\n");
      printf("\nThe OUTPUT_FILE parameter is optional - if used, all data will be sorted into a single .mca file with the filename specified by output_file.  Otherwise, individual .mca files for each root tree will be generated with matching filenames.\n\n");
      exit(-1);
    }

  readConfigFile(argv[1],"tree2mca"); //grab data from the config file
  
  if(strcmp(sort_branch,gate_branch)==0)
    same_branches=true;
  
  //initialize the output histogram
  for (int i=0;i<NSPECT;i++)
    for (int j=0;j<S32K;j++)
      outHist[i][j]=0;


  //sort list of ROOT files
  if(listMode==true)
    { 
      //read in tree list file
      if((list=fopen(inp_filename,"r"))==NULL)
        {
          printf("ERROR: Cannot open the input list file %s!\n",inp_filename);
          exit(-1);
        } 
      //scan the list file for ROOT files and put their data into the output hitogram
      while(fscanf(list,"%s",str)!=EOF)
        {
  
          inp = new TFile(str,"read");
          if((stree = (TTree*)inp->Get(tree_name))==NULL)
            {
              printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",tree_name);
              if((stree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
                {
                  printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",tree_name);
                  exit(-1);
                }
            }
          printf("Tree in %s read out.\n",str);
    
          stree->ResetBranchAddresses();
          if (!same_branches)
            stree->SetBranchAddress(sort_branch,data);
          stree->SetBranchAddress(gate_branch,gate_data);
          printf("Branch address set.\n");
          printf("Number of tree entries: %Ld\n",stree->GetEntries());

          addTreeDataToOutHist();
          
          //save results to individual .mca files if applicable
          if(output_specified==false)
            {
              if((output=fopen(strcat(str,".mca"),"w"))==NULL)
                {
                  printf("ERROR: Cannot open the output .mca file %s!\n",strcat(str,".mca"));
                  exit(-1);
                }
              for (int i=0;i<NSPECT;i++)
                fwrite(outHist[i],S32K*sizeof(int),1,output);
              fclose(output);
          
              //reset the output histogram
              for (int i=0;i<NSPECT;i++)
                for (int j=0;j<S32K;j++)
                  outHist[i][j]=0;
            }

        }   
      fclose(list);
    }
  
  //sort a single ROOT file  
  if(listMode==false)
    {
      //read in tree file
      inp = new TFile(inp_filename,"read");
      if((stree = (TTree*)inp->Get(tree_name))==NULL)
        {
          printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",tree_name);
          if((stree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
            {
               printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",tree_name);
               exit(-1);
            }
        }
      printf("Tree in %s read out.\n",inp_filename);

      stree->ResetBranchAddresses();
      if (!same_branches)
        stree->SetBranchAddress(sort_branch,data);
      stree->SetBranchAddress(gate_branch,gate_data);
      printf("Branch address set.\n");
      printf("Number of tree entries: %Ld\n",stree->GetEntries());
      
      addTreeDataToOutHist();
    }

  //save results to a single .mca file if applicable
  if(output_specified==true)
    {
      //write the output histogram to disk
      if((output=fopen(out_filename,"w"))==NULL)
        {
          printf("ERROR: Cannot open the output .mca file!\n");
          exit(-1);
        }
      for (int i=0;i<NSPECT;i++)
        fwrite(outHist[i],S32K*sizeof(int),1,output);
      fclose(output);
    }
  if((output_specified==false)&&(listMode==false))
    {
      if((output=fopen(strcat(inp_filename,".mca"),"w"))==NULL)
        {
          printf("ERROR: Cannot open the output .mca file %s!\n",strcat(inp_filename,".mca"));
          exit(-1);
        }
      for (int i=0;i<NSPECT;i++)
        fwrite(outHist[i],S32K*sizeof(int),1,output);
      fclose(output);
    }  
    
  return(0); //great success
}

//This function extracts data from the tree after 
//the tree addresses have been set and puts it in 
//the output histogram.
void addTreeDataToOutHist()
{
  for (int i=0;i<stree->GetEntries();i++)
    {
      stree->GetEntry(i);

      //if the branches are the same, copy data from one to the other
      if(same_branches)
        for (int j=0;j<NSPECT;j++)
          data[j]=(double)gate_data[j];

      if(fwhmResponse==false)
        for (int j=0;j<NSPECT;j++)
          if(gate_data[gate_leaf]==j)
            if(data[sort_leaf]<S32K)
              outHist[j][(int)(data[sort_leaf]*scaling)]++; //fill the output histogram
      if(fwhmResponse==true)
        for (int j=0;j<NSPECT;j++)
          if(gate_data[gate_leaf]==j)
            if(data[sort_leaf]<S32K)
              outHist[j][(int)(FWHM_response(data[sort_leaf]*scaling))]++; //fill the output histogram
    }

}

double FWHM_response(double e_in)
{
  double e_out,fwhm,sigma,e;
  
  if(e_in==0.)
    return e_in;
  
  e=e_in/1000.;
  // printf("e: %f, e_in: %f\n",e,e_in);
  fwhm=sqrt(fwhmF+fwhmG*e+fwhmH*e*e);
  sigma=fwhm/2.35482;
  //  printf("sigma: %f\n",sigma);
  if(sigma>0)
    e_out=randGen->Gaus(e_in,sigma);
  else
    e_out=e_in;

  return e_out;
}
