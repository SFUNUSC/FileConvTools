#include "add_mca.h"

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

  FILE *input1,*input2;
  FILE *output=NULL;
  int num_spect,num_spect2;
  int max_num_spect=0;


  if((argc!=4)&&(argc!=3))
    {
      printf("\nadd_mca input_1 input_2 output\n");
      printf("------------------------------\nAdds the two input .mca or .fmca files together spectrum by spectrum and outputs a combined .mca or .fmca file (depending on the extension provided).\n");
      printf("\nadd_mca input_list output\n");
      printf("-------------------------\nAdds the all of the .mca or .fmca files apecified in the list file together spectrum by spectrum and outputs a combined .mca or .fmca file (depending on the extension provided).\n");
      printf("The list file should contain the relative file paths to the individual .mca or .fmca files seperated by line.\n\n");
      exit(-1);
    }
    
  //initialize the output histogram
  for (int i=0;i<NSPECT;i++)
    for (int j=0;j<S32K;j++)
      outHist[i][j]=0.;

	

  //read in the .mca files
  if(argc==4) //two spectrum mode
    {
    	// read in input1
    	if((input1=fopen(argv[1],"r"))==NULL)
        {
          printf("ERROR: Cannot open the input file %s!\n",argv[1]);
          exit(-1);
        }
      const char *dot1 = strrchr(argv[1], '.'); // get the file extension
      if(strcmp(dot1 + 1,"mca")==0)
				num_spect = readMCA(input1,argv[1],inpHist1);
      else if(strcmp(dot1 + 1,"fmca")==0)
				num_spect = readFMCA(input1,argv[1],inpHist1);
      else
				{
					printf("ERROR: Improper type of input file: %s\n",argv[1]);
					printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
					exit(-1);
				}  
      fclose(input1);
      
      // read in input2
      if((input2=fopen(argv[2],"r"))==NULL)
        {
          printf("ERROR: Cannot open the input file %s!\n",argv[2]);
          exit(-1);
        }
      const char *dot2 = strrchr(argv[2], '.'); // get the file extension
      if(strcmp(dot2 + 1,"mca")==0)
				num_spect2 = readMCA(input2,argv[2],inpHist2);
      else if(strcmp(dot2 + 1,"fmca")==0)
				num_spect2 = readFMCA(input2,argv[2],inpHist2);
      else
				{
					printf("ERROR: Improper type of input file: %s\n",argv[2]);
					printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
					exit(-1);
				}  
      fclose(input2);
    	
    	if(num_spect>num_spect2)
    		max_num_spect=num_spect;
    	else
    		max_num_spect=num_spect2;
    	
      printf("Read %i spectra from file %s\n",num_spect,argv[1]);
      printf("Read %i spectra from file %s\n",num_spect,argv[2]);
      
      //append values to the output histogram
      for (int i=0;i<num_spect;i++)
        for (int j=0;j<S32K;j++)
          outHist[i][j]+=inpHist1[i][j];
      //append values to the output histogram
      for (int i=0;i<num_spect2;i++)
        for (int j=0;j<S32K;j++)
          outHist[i][j]+=inpHist2[i][j];
      
      //get the output filename
      strcpy(outName,argv[3]);
      
    }
  if(argc==3) //spectrum list mode
    {
      //open the list file
      if((input1=fopen(argv[1],"r"))==NULL)
        {
          printf("ERROR: Cannot open the input list file %s!\n",argv[1]);
          exit(-1);
        }
        
      //scan the list file for .mca files and add them to the output histogram
      while(fscanf(input1,"%s",str)!=EOF)
        {
        
        	// read in input2
        	if((input2=fopen(str,"r"))==NULL)
				    {
				      printf("ERROR: Cannot open the input file %s!\n",str);
				      exit(-1);
				    }
				  const char *dot2 = strrchr(str, '.'); // get the file extension
				  if(strcmp(dot2 + 1,"mca")==0)
						num_spect = readMCA(input2,str,inpHist2);
				  else if(strcmp(dot2 + 1,"fmca")==0)
						num_spect = readFMCA(input2,str,inpHist2);
				  else
						{
							printf("ERROR: Improper type of input file: %s\n",argv[2]);
							printf("Integer array (.mca) and float array (.fmca) files are supported.\n");
							exit(-1);
						}  
				  fclose(input2);
				  
				  
          printf("Read %i spectra from file %s\n",num_spect,str);
          
          
          if(num_spect>max_num_spect)
            max_num_spect=num_spect;
              
          //add the .mca or .fmca file values to the output histogram    
          for (int i=0;i<num_spect;i++)
            for (int j=0;j<S32K;j++)
              outHist[i][j]+=inpHist2[i][j];
          
        }
        
      fclose(input1);

      //get the output filename
      strcpy(outName,argv[2]);
        
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
  		for (int i=0;i<max_num_spect;i++)
  			for (int j=0;j<S32K;j++)
  				mcaHist[i][j]=outHist[i][j];
  		
  		//write the output histogram to disk
			for (int i=0;i<max_num_spect;i++)
				fwrite(mcaHist[i],S32K*sizeof(int),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",max_num_spect,outName);
		}
	else if(strcmp(dot + 1,"fmca")==0)
  	{
  		printf("Writing data in floating-point (.fmca) format.\n");
  		
  		//write the output histogram to disk
			for (int i=0;i<max_num_spect;i++)
				fwrite(outHist[i],S32K*sizeof(float),1,output);
			fclose(output);
			printf("Wrote %i spectra to file %s\n",max_num_spect,outName);
  	}
    
  return 0; //great success
}


