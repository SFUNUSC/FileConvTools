#include "txt2mca.h"
#include "read_config.c"

double ch[S32K],val[S32K];
char str[2][256];
FILE *input;

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
  
  FILE *output=NULL;

  if((argc!=2)&&(argc!=3))
    {
      printf("\ntxt2mca input_txt output_filename\n");
      printf("---------------------------------\nConverts the data in the specified text file into .mca format.  Assumes a two column text file, space or tab delimited, with the first column being the bin number and the second column being the bin value.\n");
      printf("\ntxt2mca parameter_file\n");
      printf("----------------------\nConverts the data in the specified text file into .mca format, using bin/value scaling and input file formatting information specified in the parameter file.\n\n");
      exit(-1);
    }

  //read in command line arguments
  if(argc==3)
    {
      strcpy(inp_filename,argv[1]);
      strcpy(out_filename,argv[2]);
    }
  else
    {
      readConfigFile(argv[1],"txt2mca"); //grab data from the parameter file
    }
  
  //read in data from the .txt file
  if((input=fopen(inp_filename,"r"))==NULL)
    {
      printf("ERROR: Cannot open the input file %s!\n",inp_filename);
      exit(-1);
    }
    
  if(file_handler_specified==true)
    readFileToArrays(file_handler);
  else
    readFileToArrays("default");
  fclose(input);
  
  
  //put data into .mca format
  for(int i=0;i<S32K;i++)
    outHist[0][i]=0;//initialize all elements to 0
  for(int i=0;i<S32K;i++)
    if((ch[i]*sort_scaling)>0.0)
      if((ch[i]*sort_scaling)<S32K)
        outHist[0][(int)round(ch[i]*sort_scaling)]+=(int)round(val[i]*val_scaling);
  
  //write the output histogram to disk
  if((output=fopen(out_filename,"w"))==NULL)
    {
      printf("ERROR: Cannot open the output .mca file!\n");
      exit(-1);
    }
  fwrite(outHist[0],S32K*sizeof(int),1,output);
  fclose(output);

  return 0; //great success
}

void readFileToArrays(const char * fileType)
{
  //regular two column data
  if(strcmp(fileType,"default")==0)
    {
      if(fgets(str[0],256,input)!=NULL)
        {
          int i=0;
          while(fscanf(input,"%s %s\n",str[0],str[1])!=EOF)
            if(atof(str[1])!=0)//ignore bin values of 0 and text strings
	            if(i<S32K)
	              {
                  ch[i]=atof(str[0]);
                  val[i]=atof(str[1]);
                  i++;
                }
        }      
      else
        {
        	printf("Wrong structure of file %s, aborting...\n",inp_filename);
          exit(-1);
        }
    }
}
