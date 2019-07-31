#include "sum_mca.h"

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
  
int main(int argc, char *argv[])
{

  FILE *input1;
  FILE *output=NULL;
  int num_spect,num_out_spect;
  int startSp,endSp,outSp;


  if((argc!=5)&&(argc!=6))
    {
      printf("\nsum_mca input_file start_sp end_sp output_file\n");
      printf("----------------------------------------------\nSums the spectra ranging from start_sp to end_sp in the input .mca/.fmca file and writes the result to a single spectrum .mca file.\n\n");
      printf("\nsum_mca input_file start_sp end_sp output_file output_sp\n");
      printf("--------------------------------------------------------\nSums the spectra ranging from start_sp to end_sp in the input .mca/.fmca file and writes the result to the output file in the specified spectrum.\n");
      printf("If the output file already exists, it will be overwritten in the specified output spectrum and preserved in the other spectra.\n\n");
      exit(-1);
    }
    
  //initialize the output histogram
  for (int i=0;i<NSPECT;i++)
    for (int j=0;j<S32K;j++)
      outHist[i][j]=0.;

      
  //read in parameters
  startSp=atoi(argv[2]);
  endSp=atoi(argv[3]);
  if((startSp<0)||(endSp<0))
    {
      printf("ERROR: Start and end spectra cannot be negative!\n");
      exit(-1);
    }
  if((endSp-startSp)<0)
    {
      int tmp=startSp;
      startSp=endSp;
      endSp=tmp;
      printf("WARNING: Start and end spectra specified in the wrong order.  Summing from spectrum %i to %i.\n",startSp,endSp);
    }
  strcpy(outName,argv[4]);
  outSp=-1;
  if(argc>=6)
    outSp=atoi(argv[5]);
  if(outSp>=NSPECT)
    {
      printf("ERROR: output spectrum number is larger than NSPECT (%i).  Reduce the output spectrum value or increase NSPECT defined in sum_mca.h to proceed.\n",NSPECT);
      exit(-1);
    }
  num_spect=0;
  if(outSp>=0)
    {
      num_out_spect=outSp+1;
    }
  else
    {
      num_out_spect=1;
    } 

  // read in input1
  if((input1=fopen(argv[1],"r"))==NULL)
    {
      printf("ERROR: Cannot open the input file %s!\n",argv[1]);
      exit(-1);
    }
  
  const char *dot1 = strrchr(argv[1], '.'); // get the file extension
  if(strcmp(dot1 + 1,"mca")==0)
    num_spect=readMCA(input1,argv[1],inpHist1);
  else if(strcmp(dot1 + 1,"fmca")==0)
    num_spect=readFMCA(input1,argv[1],inpHist1);
  else
    {
      printf("ERROR: Improper type of input file: %s\n",argv[1]);
      printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
      exit(-1);
    }  
  fclose(input1);
  
  if(startSp>=(num_spect-1))
    startSp=num_spect-1;
  if(endSp>=(num_spect-1))
    endSp=num_spect-1;

  printf("Summing from spectrum %i to %i, writing to spectrum %i in the output file.\n",startSp,endSp,outSp);

  // read in output, if it exists
  if((input1=fopen(outName,"r"))==NULL)
    {
      printf("Output file %s does not exist, creating this file.\n",outName);
    }
  else
    {
      const char *dot1 = strrchr(outName, '.'); // get the file extension
      if(strcmp(dot1 + 1,"mca")==0)
        num_out_spect=readMCA(input1,outName,outHist);
      else if(strcmp(dot1 + 1,"fmca")==0)
        num_out_spect=readFMCA(input1,outName,outHist);
      else
        {
          printf("ERROR: Improper type of output file: %s\n",outName);
          printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
          exit(-1);
        }
      fclose(input1);
    }

  if((num_out_spect<0)||(num_out_spect>NSPECT))
    {
      printf("ERROR: Number of output spectra out of bounds (%i)!\n",num_out_spect);
    }

  //zero output spectrum
  for (int j=0;j<S32K;j++)
    {
      if(outSp<0){
        outHist[0][j]=0.;
      }else if(outSp<num_out_spect){
        outHist[outSp][j]=0.;
      }else if(outSp<NSPECT){
        num_out_spect=outSp+1;
        outHist[outSp][j]=0.;
      }else{
        printf("ERROR: output spectrum number is larger than NSPECT (%i).  Reduce the output spectrum value or increase NSPECT defined in sum_mca.h to proceed.\n",NSPECT);
        exit(-1);
      }
    }
     
  //append values to the output spectrum
  for (int i=startSp;i<=endSp;i++)
    for (int j=0;j<S32K;j++)
      if(outSp<0){
        outHist[0][j]+=inpHist1[i][j];
      }else if(outSp<num_out_spect){
        outHist[outSp][j]+=inpHist1[i][j];
      }
      


  //open the output file   
  if((output=fopen(outName,"w"))==NULL)
    {
      printf("ERROR: Cannot open the output file: %s!\n",outName);
      exit(-1);
    }
    
  const char *dot = strrchr(outName, '.'); // get output file extension
  if(strcmp(dot + 1,"mca")==0)
  	{
  		printf("Writing data in integer (.mca) format.\n");
  		
  		//convert data to integer
  		for (int i=0;i<NSPECT;i++)
  			for (int j=0;j<S32K;j++)
  				mcaHist[i][j]=outHist[i][j];
  		
  		//write the output histogram to disk
			for (int i=0;i<num_out_spect;i++)
				fwrite(mcaHist[i],S32K*sizeof(int),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",num_out_spect,outName);
		}
	else if(strcmp(dot + 1,"fmca")==0)
  	{
  		printf("Writing data in floating-point (.fmca) format.\n");
  		
  		//write the output histogram to disk
			for (int i=0;i<num_out_spect;i++)
				fwrite(outHist[i],S32K*sizeof(float),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",num_out_spect,outName);
  	}

    
  return 0; //great success
}
