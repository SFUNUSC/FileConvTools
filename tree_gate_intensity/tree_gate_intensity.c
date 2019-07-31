#include "tree_gate_intensity.h"
#include "read_config.c"

//forward declarations
void getIntensities();

char str[256];
long int numEv[NSPECT];//number of events in each gate
//Creating an instance of TApplication
//This is evidently needed for auto-loading of ROOT libraries, 
//otherwise the program may crash upon execution depending on how ROOT
//is set up.
//http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14064
int ac;
char* av[10];
TApplication *theApp=new TApplication("App", &ac, av);
  
int main(int argc, char *argv[])
{

  FILE *list;
  TFile *inp;

  if(argc!=2)
    {
      printf("\ntree_gate_intensity parameter_file\n");
      printf("-----------------------------\nChecks the number of events in the specified gates in the specified ROOT tree.\n\n");
      exit(-1);
    }

  readConfigFile(argv[1],"tree2tree"); //grab data from the parameter file
  
  memset(numEv,0,sizeof(numEv));
  
  //WILL WANT TO IMPLEMENT LIST SORTING LATER
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
          if((gtree = (TTree*)inp->Get(gate_tree_name))==NULL)
            {
              printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",gate_tree_name);
              if((gtree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
                {
                  printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",gate_tree_name);
                  exit(-1);
                }
            }
          printf("Tree in %s read out.\n",str);
          
          if((gateLeaf[0] = gtree->GetLeaf(gate_path[0]))==NULL)
            if((gateBranch[0] = gtree->GetBranch(gate_path[0]))==NULL)
              {
                printf("ERROR: Gate data path '%s' doesn't correspond to a branch or leaf in the tree!\n",gate_path[0]);
                exit(-1);
              } 
          if(gateLeaf[0]==NULL)
            gateLeaf[0] = (TLeaf*)gateBranch[0]->GetListOfLeaves()->First(); //get the first leaf from the specified branch
          
          if(use_custom_gates == 4)
            {
              for(int i=1;i<4;i++)
                {
                  if((gateLeaf[i] = gtree->GetLeaf(gate_path[i]))==NULL)
                  if((gateBranch[i] = gtree->GetBranch(gate_path[i]))==NULL)
                    {
                      printf("ERROR: Gate data path '%s' doesn't correspond to a branch or leaf in the tree!\n",gate_path[i]);
                      exit(-1);
                    } 
                if(gateLeaf[i]==NULL)
                  gateLeaf[i] = (TLeaf*)gateBranch[i]->GetListOfLeaves()->First(); //get the first leaf from the specified branch
                }
              
            }
          printf("Paths to sort and gate data set.\n");

          getIntensities();

        }   
      fclose(list);
    }
  
  
  
  //sort a single ROOT file
  if(listMode==false)
    {
      //read in tree file
      inp = new TFile(inp_filename,"read");
      if((gtree = (TTree*)inp->Get(gate_tree_name))==NULL)
        {
          printf("The specified tree named %s doesn't exist, trying default name 'tree'.\n",gate_tree_name);
          if((gtree = (TTree*)inp->Get("tree"))==NULL)//try the default tree name
            {
              printf("ERROR: The specified tree named %s (within the ROOT file) cannot be opened!\n",gate_tree_name);
              exit(-1);
            }
        }
      printf("Tree in %s read out.\n",inp_filename);

      if((gateLeaf[0] = gtree->GetLeaf(gate_path[0]))==NULL)
        if((gateBranch[0] = gtree->GetBranch(gate_path[0]))==NULL)
          {
            printf("ERROR: Gate data path '%s' doesn't correspond to a branch or leaf in the tree!\n",gate_path[0]);
            exit(-1);
          }
      if(gateLeaf[0]==NULL)
        gateLeaf[0] = (TLeaf*)gateBranch[0]->GetListOfLeaves()->First(); //get the first leaf from the specified branch
      printf("Paths to sort and gate data set.\n");
      
      getIntensities();

    }
    
  printf("Number of events in each gate:\nGate #   N\n");
  for (int i=0;i<num_custom_gates;i++)
    printf("%i    %li\n",i+1,numEv[i]);
  if(use_custom_gates==2)
    printf("NOTE: some entries may be 0 since double counting is not allowed.\n");
    
  return 0; //great success
}

void getIntensities()
{
  //sort the tree data
  Double_t gate_value[4];
  long long int entries;
  
  entries=gtree->GetEntries();
  printf("Number of entries in input tree: %Ld\n",entries);
  
  for (int i=0;i<entries;i++)
    {
      gtree->GetEntry(i);
      
      //printf("Values in gate leaf: %i\n",gateLeaf->GetNdata()); 
      
      if(use_gate_weights==true)//custom gates used with weights
        {
          if(use_custom_gates==1)//1D gate
            for(int j=0; j<gateLeaf[0]->GetNdata(); j++) //deal with multiple fold events
              {
                gate_value[0] = gateLeaf[0]->GetValue(j);
                for (int k=0;k<num_custom_gates;k++)
                  if(valueInRange(gate_value[0],custom_gates[k][0],custom_gates[k][1]))
                    numEv[k]++;
              }
            
          if(use_custom_gates==2)//2D gate
            for(int j=0; j<gateLeaf[0]->GetNdata(); j++) //deal with multiple fold events
              for(int k=j+1; k<gateLeaf[0]->GetNdata(); k++)
                {
                  gate_value[0] = gateLeaf[0]->GetValue(j);
                  gate_value[1] = gateLeaf[0]->GetValue(k);
                  for (int l=0;l<num_custom_gates;l++)
                    if((valueInRange(gate_value[0],custom_gates[l][0],custom_gates[l][1]))&&(valueInRange(gate_value[1],custom_gates[l][2],custom_gates[l][3])))
                      {
                        numEv[l]++;
                        break;
                      }
                    else if((valueInRange(gate_value[0],custom_gates[l][2],custom_gates[l][3]))&&(valueInRange(gate_value[1],custom_gates[l][0],custom_gates[l][1])))
                      {
                        numEv[l]++;
                        break;
                      }
                }

          if(use_custom_gates==4)//4D gate
            {
              /*printf("ndata: %i\n",gateLeaf[0]->GetNdata());
              printf("values: %f %f, %f %f\n",gateLeaf[0]->GetValue(0),gateLeaf[1]->GetValue(0),gateLeaf[2]->GetValue(1),gateLeaf[3]->GetValue(1));
              getc(stdin);*/
              if(gateLeaf[0]->GetNdata() >= 2) 
                {
                  gate_value[0] = gateLeaf[0]->GetValue(0);
                  gate_value[1] = gateLeaf[1]->GetValue(0);
                  gate_value[2] = gateLeaf[2]->GetValue(1);
                  gate_value[3] = gateLeaf[3]->GetValue(1);
                  for (int n=0;n<num_custom_gates;n++)
                    if((valueInRange(gate_value[0],custom_gates[n][0],custom_gates[n][1]))&&(valueInRange(gate_value[1],custom_gates[n][2],custom_gates[n][3]))&&(valueInRange(gate_value[2],custom_gates[n][4],custom_gates[n][5]))&&(valueInRange(gate_value[3],custom_gates[n][6],custom_gates[n][7])))
                      {
                        numEv[n]++;
                        break;
                      }
                    else if((valueInRange(gate_value[0],custom_gates[n][4],custom_gates[n][5]))&&(valueInRange(gate_value[1],custom_gates[n][6],custom_gates[n][7]))&&(valueInRange(gate_value[2],custom_gates[n][0],custom_gates[n][1]))&&(valueInRange(gate_value[3],custom_gates[n][2],custom_gates[n][3])))
                      {
                        numEv[n]++;
                        break;
                      }
                }
              else
                {
                  printf("Not enough entries for 4D gate!\n");
                }
              
            }
            
        }
    }
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
