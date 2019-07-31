#include <string.h>

void readConfigFile(const char *fileName) {

  if ((config = fopen(fileName, "r")) == NULL) {
    printf("ERROR: Cannot open the parameter file %s!\n", fileName);
    exit(-1);
  }

  while (!(feof(config))) // go until the end of file is reached
  {
    if (fgets(cfgstr, 256, config) != NULL) {
      if (sscanf(cfgstr, "%s %s", str1, str2) == 2) // single parameter data
      {
        if (strcmp(str1, "SORT_PATH") == 0)
          strcpy(sort_path, str2);
        if (strcmp(str1, "WEIGHT_PATH") == 0) {
          strcpy(weight_path, str2);
          use_weights = true;
        }

        if (strcmp(str1, "INPUT_FILE") == 0) {
          listMode = false;
          strcpy(inp_filename, str2);
        }
        if (strcmp(str1, "INPUT_FILE_LIST") == 0) {
          listMode = true;
          strcpy(inp_filename, str2);
        }
        if (strcmp(str1, "TREE_NAME") ==
            0) // use this to put the game tree for sort and gate
        {
          strcpy(sort_tree_name, str2);
        }
        if (strcmp(str1, "SORT_TREE_NAME") == 0)
          strcpy(sort_tree_name, str2);
        if (strcmp(str1, "OUTPUT_FILE") == 0) {
          output_specified = true;
          strcpy(out_filename, str2);
        }
        // group map stuff
        if ((strcmp(str1, "GROUP_MAP_PATH") == 0) ||
            (strcmp(str1, "RING_MAP_PATH") == 0))
          strcpy(group_file, str2);
        if (strcmp(str1, "POS_PATH") == 0)
          strcpy(pos_path, str2);
        if (strcmp(str1, "COL_PATH") == 0)
          strcpy(col_path, str2);
        if (strcmp(str1, "CSI_PATH") == 0)
          strcpy(csi_path, str2);
        if (strcmp(str1, "DG1") == 0)
          dg1 = atof(str2);
        if (strcmp(str1, "DG2") == 0)
          dg2 = atof(str2);
        if (strcmp(str1, "DG3") == 0)
          dg3 = atof(str2);
        if (strcmp(str1, "DG4") == 0)
          dg4 = atof(str2);
        if (strcmp(str1, "DG5") == 0)
          dg5 = atof(str2);
      }
      if (sscanf(cfgstr, "%s %s", str1, str2) == 1) // only one item on line
        if (strcmp(str1, "<---END_OF_PARAMETERS--->") == 0)
          break;
    }
  }
  fclose(config);

  // Report parameters from the config file
  if (listMode == false)
    printf("Input tree file: %s\n", inp_filename);
  if (listMode == true)
    printf("Input list file: %s\n", inp_filename);
  printf("Sorting from leaf with path: %s in tree: %s\n", sort_path,
         sort_tree_name);
  if(use_weights)
    printf("Using weights from leaf with path: %s in tree: %s\n", weight_path,
         sort_tree_name);
  else
    printf("Not using weights.\n");
  if (output_specified == true)
    printf("Will save output data to file: %s\n", out_filename);
  if (output_specified == false)
    printf("Will save output data to individual files (input filenames + "
           "'.mca').\n");
  printf("Doppler shift groups:\n");
  printf("1: %.3f < D\n", dg1);
  printf("2: %.3f < D <= %.3f\n", dg2, dg1);
  printf("3: %.3f < D <= %.3f\n", dg3, dg2);
  printf("4: %.3f < D <= %.3f\n", dg4, dg3);
  printf("5: %.3f < D <= %.3f\n", dg5, dg4);
  printf("6: D <= %.3f\n", dg5);

  printf("Finished reading parameter file...\n");
}
