#include "contract_mca.h"



int readMCA(FILE* inp,char* filename,float inpHist[NSPECT][S32K])
{
	int num_spect=0;
  for (int i=0; i<NSPECT; i++)
    if(fread(mcaHist[i],S32K*sizeof(int),1,inp)!=1)
      {
      	num_spect=i;
				break; // dont read in more spectra than there are in the input file
				/* printf("ERROR: Error reading file %s!\n",filename); */
				/* printf("For i=%d fread(mcaHist[i],S32K*sizeof(int),1,inp)=%lu\n",i,fread(mcaHist[i],S32K*sizeof(int),1,inp)); */
				/* exit(-1); */
      }
 
  for (int i=0; i<NSPECT; i++)
    for (int j=0; j<S32K; j++)
      inpHist[i][j]=(float)mcaHist[i][j];

  return num_spect;
}

/*******************************************************************************/
int readFMCA(FILE* inp,char* filename,float inpHist[NSPECT][S32K])
{
	int num_spect=0;
  for (int i=0; i<NSPECT; i++)
    if(fread(inpHist[i],S32K*sizeof(float),1,inp)!=1)
      {
      	num_spect=i;
				break; // dont read in more spectra than there are in the input file
				/* printf("ERROR: Error reading file %s!\n",filename); */
				/* printf("For i=%d fread(inpHist[i],S32K*sizeof(float),1,inp)=%lu\n",i,fread(inpHist[i],S32K*sizeof(float),1,inp)); */
				/* exit(-1); */
      }

  return num_spect;
}

/*******************************************************************************/
int main(int argc, char *argv[])
{

  FILE *input;
  FILE *output=NULL;
  int num_spect;
  int cntr;


  if(argc!=4)
    {
      printf("\ncontract_mca input_file contraction_factor output_file\n");
      printf("------------------------------------------------------\nContracts the input .mca or .fmca file by the specified contraction factor, and outputs an .mca or .fmca file (depending on the extension provided).\n\n");
      exit(-1);
    }

  //read in command line arguments
  cntr=atoi(argv[2]);
  printf("Using a contraction factor of %i.\n",cntr);
  
  // read in input file
	if((input=fopen(argv[1],"r"))==NULL)
    {
      printf("ERROR: Cannot open the input file %s!\n",argv[1]);
      exit(-1);
    }
  const char *dot1 = strrchr(argv[1], '.'); // get the file extension
  if(strcmp(dot1 + 1,"mca")==0)
		num_spect = readMCA(input,argv[1],inpHist);
  else if(strcmp(dot1 + 1,"fmca")==0)
		num_spect = readFMCA(input,argv[1],inpHist);
  else
		{
			printf("ERROR: Improper type of input file: %s\n",argv[1]);
			printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
			exit(-1);
		}
	printf("Read %i spectra from file %s\n",num_spect,argv[1]);
  fclose(input);
  
  //initialize the output histogram
  for (int i=0;i<num_spect;i++)
    for (int j=0;j<S32K;j++)
      outHist[i][j]=0.;
  
  //generate a contracted output histogram
  for (int i=0;i<num_spect;i++)
    for (int j=0;j<((int)(S32K/(float)cntr));j++)
      for (int k=0; k<cntr; k++)
        outHist[i][j]+=inpHist[i][cntr*j+k];
      
  //open the output file
  strcpy(outName,argv[3]);
  if((output=fopen(outName,"w"))==NULL)
    {
      printf("ERROR: Cannot open the output .mca file!\n");
      exit(-1);
    }
  const char *dot = strrchr(outName, '.'); // get output file extension
  if(strcmp(dot + 1,"mca")==0)
  	{
  		printf("Writing data in integer (.mca) format.\n");
  		
  		//convert data to integer
  		for (int i=0;i<num_spect;i++)
  			for (int j=0;j<S32K;j++)
  				mcaHist[i][j]=outHist[i][j];
  		
  		//write the output histogram to disk
			for (int i=0;i<num_spect;i++)
				fwrite(mcaHist[i],S32K*sizeof(int),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",num_spect,outName);
		}
	else if(strcmp(dot + 1,"fmca")==0)
  	{
  		printf("Writing data in floating-point (.fmca) format.\n");
  		
  		//write the output histogram to disk
			for (int i=0;i<num_spect;i++)
				fwrite(outHist[i],S32K*sizeof(float),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",num_spect,outName);
  	}

  return 0; //great success
}
