#include "tree2mca_gated.h"
#include "read_config.c"

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

  FILE *list, *output;
  TFile *inp;
  randGen = new TRandom3();

  if(argc!=2)
    {
      printf("\ntree2mca_gated parameter_file\n");
      printf("-----------------------------\nTakes the data in the specified branch and leaf of the specified ROOT tree(s) and sorts it to .mca file(s) with spectra gated on the data in the specified gate branch and leaf.\n\n");
      exit(-1);
    }

  readConfigFile(argv[1],"tree2mca_gated"); //grab data from the parameter file
  
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
          if((stree = (TTree*)inp->Get(sort_tree_name))==NULL)
            {
              printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",sort_tree_name);
              if((stree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
                {
                  printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",sort_tree_name);
                  exit(-1);
                }
            }
          if((gtree = (TTree*)inp->Get(gate_tree_name))==NULL)
            {
              printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",gate_tree_name);
              if((gtree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
                {
                  printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",gate_tree_name);
                  exit(-1);
                }
            }
          printf("Tree(s) in %s read out.\n",str);
          
          if((sortLeaf = stree->GetLeaf(sort_path))==NULL)
            if((sortBranch = stree->GetBranch(sort_path))==NULL)
              {
                printf("ERROR: Sort data path '%s' doesn't correspond to a branch or leaf in the tree!\n",sort_path);
                exit(-1);
              }
          if((gateLeaf = gtree->GetLeaf(gate_path))==NULL)
            if((gateBranch = gtree->GetBranch(gate_path))==NULL)
              {
                printf("ERROR: Gate data path '%s' doesn't correspond to a branch or leaf in the tree!\n",gate_path);
                exit(-1);
              }
          if(sortLeaf==NULL)
            sortLeaf = (TLeaf*)sortBranch->GetListOfLeaves()->First(); //get the first leaf from the specified branch       
          if(gateLeaf==NULL)
            gateLeaf = (TLeaf*)gateBranch->GetListOfLeaves()->First(); //get the first leaf from the specified branch
          printf("Paths to sort and gate data set.\n");

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
      if((stree = (TTree*)inp->Get(sort_tree_name))==NULL)
        {
          printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",sort_tree_name);
          if((stree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
            {
               printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",sort_tree_name);
               exit(-1);
            }
        }
      if((gtree = (TTree*)inp->Get(gate_tree_name))==NULL)
        {
          printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",gate_tree_name);
          if((gtree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
            {
              printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",gate_tree_name);
              exit(-1);
            }
        }
      printf("Tree(s) in %s read out.\n",inp_filename);

      if((sortLeaf = stree->GetLeaf(sort_path))==NULL)
        if((sortBranch = stree->GetBranch(sort_path))==NULL)
          {
            printf("ERROR: Sort data path '%s' doesn't correspond to a branch or leaf in the tree!\n",sort_path);
            exit(-1);
          }
      if((gateLeaf = gtree->GetLeaf(gate_path))==NULL)
        if((gateBranch = gtree->GetBranch(gate_path))==NULL)
          {
            printf("ERROR: Gate data path '%s' doesn't correspond to a branch or leaf in the tree!\n",gate_path);
            exit(-1);
          }
      if(sortLeaf==NULL)
        sortLeaf = (TLeaf*)sortBranch->GetListOfLeaves()->First(); //get the first leaf from the specified branch
      if(gateLeaf==NULL)
        gateLeaf = (TLeaf*)gateBranch->GetListOfLeaves()->First(); //get the first leaf from the specified branch
      printf("Paths to sort and gate data set.\n");
      
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
    
  return 0; //great success
}

//This function extracts data from the tree after 
//the tree addresses have been set and puts it in 
//the output histogram.
void addTreeDataToOutHist()
{
  Double_t sort_value,gate_value;
  long long int entries;
  double histVal;
  
  entries=stree->GetEntries();
  if(gtree->GetEntries()<entries)
    entries=gtree->GetEntries();
  printf("Number of entries in tree(s): %Ld\n",entries);

  for (int i=0;i<entries;i++)
    {
      stree->GetEntry(i);
      gtree->GetEntry(i);
      
      //printf("Values in sort leaf: %i\n",sortLeaf->GetNdata());
      //printf("Values in gate leaf: %i\n",gateLeaf->GetNdata()); 
      
      for(int j=0; j<sortLeaf->GetNdata(); j++) //deal with multiple fold events
        {
          if(sortLeaf->GetNdata()==gateLeaf->GetNdata())
            {
              sort_value = sortLeaf->GetValue(j);
              gate_value = gateLeaf->GetValue(j);
            }
          else
            {
              sort_value = sortLeaf->GetValue(j);
              gate_value = gateLeaf->GetValue(0);//use the first gate value
            }

          if(use_custom_gates==0)//no custom weights
            {
              for (int k=0;k<NSPECT;k++)
                if((int)(gate_value*gate_scaling + gate_shift)==k)
                  {
                    if(fwhmResponse==false)
                      histVal=sort_value*sort_scaling + sort_shift;
                    else
                    	{
                      	histVal=FWHM_response(sort_value);
                      	histVal=histVal*sort_scaling + sort_shift;
                      }
                    if(histVal>=0.0)
                      if(histVal<S32K)
                        outHist[k][(int)(histVal)]++; //fill the output histogram
                  }
            }
          else if(use_gate_weights==false)//custom gates used without weights
            {
              for (int k=0;k<NSPECT;k++)
                if(valueInRange(gate_value,custom_gates[k][0],custom_gates[k][1]))
                  {
                    if(fwhmResponse==false)
                      histVal=sort_value*sort_scaling + sort_shift;
                    else
                    	{
                      	histVal=FWHM_response(sort_value);
                      	histVal=histVal*sort_scaling + sort_shift;
                      }
                    if(histVal>=0.0)
                      if(histVal<S32K)
                        outHist[k][(int)(histVal)]++; //fill the output histogram
                  }
            }
          else//custom gates used with weights
            {
              for (int k=0;k<NSPECT;k++)
                if(valueInRange(gate_value,custom_gates[k][0],custom_gates[k][1]))
                  if(randGen->Uniform()<gate_weight[k])
                    {
                      if(fwhmResponse==false)
                        histVal=sort_value*sort_scaling + sort_shift;
                      else
                        histVal=FWHM_response(sort_value);
                      	histVal=histVal*sort_scaling + sort_shift;
                      if(histVal>=0.0)
                        if(histVal<S32K)
                          outHist[k][(int)(histVal)]++; //fill the output histogram
                    }
            }
        }
    }

}

double FWHM_response(double ch_in)
{
  if(ch_in==0.)
    return ch_in;

  double ch_out,fwhm,sigma,ch;
  double roll=randGen->Uniform();
  
  if((wL>0.0)||(wH>0.0))//gaussian with exponential tail(s)
    {
      if(roll<wG)//gaussian response
        {
          ch=ch_in/1000.;
          fwhm=sqrt(fwhmF*fwhmF + fwhmG*fwhmG*ch + fwhmH*fwhmH*ch*ch);
          sigma=fwhm/2.35482;
          if(sigma>0)
            ch_out=randGen->Gaus(ch_in,sigma);
          else
            ch_out=ch_in;
        }
      else if(roll<(wG+wH))//high energy exponential response
        {
          ch_out = ch_in + randGen->Exp(fwhmTauH);
        }
      else//low energy exponential response
        {
          ch_out = ch_in - randGen->Exp(fwhmTauL);
        }
    }
  else//gaussian only
    {
      ch=ch_in/1000.;
      fwhm=sqrt(fwhmF*fwhmF + fwhmG*fwhmG*ch + fwhmH*fwhmH*ch*ch);
      sigma=fwhm/2.35482;
      if(sigma>0)
        ch_out=randGen->Gaus(ch_in,sigma);
      else
        ch_out=ch_in;
    }

  return ch_out;
}

//checks whether a value falls within the range of two bounds (lower bound inclusive)
//the order of the bounds doesn't matter
bool valueInRange(double value, double bound1, double bound2)
{

  if(bound1>bound2)
    {
      if((value>=bound2)&&(value<bound1))
        return true;
      else
        return false;
    }
  else if(bound2>bound1)
    {
      if((value>=bound1)&&(value<bound2))
        return true;
      else
        return false;
    }
  else if(bound1==bound2)
    {
      if(value==bound1)
        return true;
      else
        return false;
    }
    
  return false;
}
