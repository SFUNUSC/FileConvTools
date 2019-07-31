#include "tree2mca_group.h"
#include "read_config.c"

char str[256];

int main(int argc, char *argv[]) {
  // Creating an instance of TApplication
  // This is evidently needed for auto-loading of ROOT libraries,
  // otherwise the program may crash upon execution depending on how ROOT
  // is set up.
  // http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=14064
  int ac;
  char *av[10];
  theApp = new TApplication("App", &ac, av);

  FILE *list, *output, *output1;
  TFile *inp;
  randGen = new TRandom3();

  if (argc != 2) {
    printf("\ntree2mca_group parameter_file\n");
    printf("-----------------------\nSorts TIGRESS add-back spectra gated by "
           "TIGRESS-CsI Doppler shift groups defined via a group map.\n\n");
    exit(-1);
  }

  readConfigFile(argv[1], "tree2mca_group"); // grab data from the parameter
                                             // file

  // initialize histograms
  for (int i = 0; i < NSPECT; i++)
    for (int j = 0; j < S32K; j++) {
      dOutHist[i][j] = 0.; // initialize all elements to 0
      groupHist[i][j] = 0;
    }

  // sort list of ROOT files
  if (listMode == true) {
    // read in tree list file
    if ((list = fopen(inp_filename, "r")) == NULL) {
      printf("ERROR: Cannot open the input list file %s!\n", inp_filename);
      exit(-1);
    }
    // scan the list file for ROOT files and put their data into the output
    // hitogram
    while (fscanf(list, "%s", str) != EOF) {
      inp = new TFile(str, "read");
      if ((stree = (TTree *)inp->Get(sort_tree_name)) == NULL) {
        printf("The specified tree named %s doesn't exist, trying default name "
               "'tree'.\n",
               sort_tree_name);
        if ((stree = (TTree *)inp->Get("tree")) ==
            NULL) // try the default tree name
        {
          printf("ERROR: The specified tree named %s (within the ROOT file) "
                 "cannot be opened!\n",
                 sort_tree_name);
          exit(-1);
        }
      }
      printf("Tree in %s read out.\n", str);

      // sort leaf (energy)
      if ((sortLeaf = stree->GetLeaf(sort_path)) == NULL)
        if ((sortBranch = stree->GetBranch(sort_path)) == NULL) {
          printf("ERROR: Sort data path '%s' doesn't correspond to a branch or "
                 "leaf in the tree!\n",
                 sort_path);
          exit(-1);
        }
      if (sortLeaf == NULL)
        sortLeaf =
            (TLeaf *)sortBranch->GetListOfLeaves()
                ->First(); // get the first leaf from the specified branch

      // weight leaf
      if ((weightLeaf = stree->GetLeaf(weight_path)) == NULL)
        if ((weightBranch = stree->GetBranch(weight_path)) == NULL) {
          printf("ERROR: Weight data path '%s' doesn't correspond to a branch "
                 "or leaf in the tree!\n",
                 weight_path);
          exit(-1);
        }
      if (weightLeaf == NULL)
        weightLeaf =
            (TLeaf *)weightBranch->GetListOfLeaves()
                ->First(); // get the first leaf from the specified branch

      // position leaf
      if ((posLeaf = stree->GetLeaf(pos_path)) == NULL)
        if ((posBranch = stree->GetBranch(pos_path)) == NULL) {
          printf("ERROR: Pos data path '%s' doesn't correspond to a branch or "
                 "leaf in the tree!\n",
                 pos_path);
          exit(-1);
        }
      if (posLeaf == NULL)
        posLeaf = (TLeaf *)posBranch->GetListOfLeaves()
                      ->First(); // get the first leaf from the specified branch

      // color leaf
      if ((colLeaf = stree->GetLeaf(col_path)) == NULL)
        if ((colBranch = stree->GetBranch(col_path)) == NULL) {
          printf("ERROR: Col data path '%s' doesn't correspond to a branch or "
                 "leaf in the tree!\n",
                 col_path);
          exit(-1);
        }
      if (colLeaf == NULL)
        colLeaf = (TLeaf *)colBranch->GetListOfLeaves()
                      ->First(); // get the first leaf from the specified branch

      // csi leaf
      if ((csiLeaf = stree->GetLeaf(csi_path)) == NULL)
        if ((csiBranch = stree->GetBranch(csi_path)) == NULL) {
          printf("ERROR: CsI data path '%s' doesn't correspond to a branch or "
                 "leaf in the tree!\n",
                 csi_path);
          exit(-1);
        }
      if (csiLeaf == NULL)
        csiLeaf = (TLeaf *)csiBranch->GetListOfLeaves()
                      ->First(); // get the first leaf from the specified branch

      printf("Paths to sort data set.\n");
      printf("Number of tree entries: %Ld\n", stree->GetEntries());
      /* printf("%Ld\n",stree->GetEntries()); */

      readGroupMap();
      addTreeDataToOutHist();

      // save results to individual .mca files if applicable
      if (output_specified == false) {
        if ((output = fopen(strcat(str, ".mca"), "w")) == NULL) {
          printf("ERROR: Cannot open the output .mca file %s!\n",
                 strcat(str, ".mca"));
          exit(-1);
        }
        if ((output1 = fopen("groupHist.mca", "w")) == NULL) {
          printf("ERROR: Cannot open the group histogram .mca file\n");
          exit(-1);
        }
        for (int i = 0; i < NSPECT; i++) {
          fwrite(dOutHist[i], S32K * sizeof(float), 1, output);
          fwrite(groupHist[i], S32K * sizeof(int), 1, output1);
        }
        fclose(output);
        fclose(output1);

        // reset the output histogram
        for (int i = 0; i < NSPECT; i++)
          for (int j = 0; j < S32K; j++) {
            dOutHist[i][j] = 0.;
            groupHist[i][j] = 0;
          }
      }
    }
    fclose(list);
  }

  // sort a single ROOT file
  if (listMode == false) {
    // read in tree file
    inp = new TFile(inp_filename, "read");
    if ((stree = (TTree *)inp->Get(sort_tree_name)) == NULL) {
      printf("The specified tree named %s doesn't exist, trying default name "
             "'tree'.\n",
             sort_tree_name);
      if ((stree = (TTree *)inp->Get("tree")) ==
          NULL) // try the default tree name
      {
        printf("ERROR: The specified tree named %s (within the ROOT file) "
               "cannot be opened!\n",
               sort_tree_name);
        exit(-1);
      }
    }
    printf("Tree in %s read out.\n", inp_filename);

    if ((sortLeaf = stree->GetLeaf(sort_path)) == NULL)
      if ((sortBranch = stree->GetBranch(sort_path)) == NULL) {
        printf("ERROR: Sort data path '%s' doesn't correspond to a branch or "
               "leaf in the tree!\n",
               sort_path);
        exit(-1);
      }
    if (sortLeaf == NULL)
      sortLeaf = (TLeaf *)sortBranch->GetListOfLeaves()
                     ->First(); // get the first leaf from the specified branch

    // weight leaf
    if ((weightLeaf = stree->GetLeaf(weight_path)) == NULL)
      if ((weightBranch = stree->GetBranch(weight_path)) == NULL) {
        printf("ERROR: Weight data path '%s' doesn't correspond to a branch or "
               "leaf in the tree!\n",
               weight_path);
        exit(-1);
      }
    if (weightLeaf == NULL)
      weightLeaf =
          (TLeaf *)weightBranch->GetListOfLeaves()
              ->First(); // get the first leaf from the specified branch

    if ((posLeaf = stree->GetLeaf(pos_path)) == NULL)
      if ((posBranch = stree->GetBranch(pos_path)) == NULL) {
        printf("ERROR: Pos data path '%s' doesn't correspond to a branch or "
               "leaf in the tree!\n",
               pos_path);
        exit(-1);
      }
    if (posLeaf == NULL)
      posLeaf = (TLeaf *)posBranch->GetListOfLeaves()
                    ->First(); // get the first leaf from the specified branch

    if ((colLeaf = stree->GetLeaf(col_path)) == NULL)
      if ((colBranch = stree->GetBranch(col_path)) == NULL) {
        printf("ERROR: Col data path '%s' doesn't correspond to a branch or "
               "leaf in the tree!\n",
               col_path);
        exit(-1);
      }
    if (colLeaf == NULL)
      colLeaf = (TLeaf *)colBranch->GetListOfLeaves()
                    ->First(); // get the first leaf from the specified branch

    if ((csiLeaf = stree->GetLeaf(csi_path)) == NULL)
      if ((csiBranch = stree->GetBranch(csi_path)) == NULL) {
        printf("ERROR: CsI data path '%s' doesn't correspond to a branch or "
               "leaf in the tree!\n",
               csi_path);
        exit(-1);
      }
    if (csiLeaf == NULL)
      csiLeaf = (TLeaf *)csiBranch->GetListOfLeaves()
                    ->First(); // get the first leaf from the specified branch

    printf("Paths to sort data set.\n");
    printf("Number of tree entries: %Ld\n", stree->GetEntries());

    readGroupMap();
    addTreeDataToOutHist();
  }

  // write the output histogram to disk
  if ((output = fopen(out_filename, "w")) == NULL) {
    printf("ERROR: Cannot open the output .fmca file!\n");
    exit(-1);
  }
  if ((output1 = fopen("groupHist.mca", "w")) == NULL) {
    printf("ERROR: Cannot open the group histogram .mca file!\n");
    exit(-1);
  }

  for (int i = 0; i < NSPECT; i++) {
    fwrite(dOutHist[i], S32K * sizeof(float), 1, output);
    fwrite(groupHist[i], S32K * sizeof(int), 1, output1);
  }
  fclose(output);
  fclose(output1);

  return 0; // great success
}

// This function extracts data from the tree after
// the tree addresses have been set and puts it in
// the output histogram.
void addTreeDataToOutHist() {
  Double_t sort_value, weight_value;
  Int_t pos, col, csi, group;
  double histVal;

  for (int i = 0; i < stree->GetEntries(); i++) {
    stree->GetEntry(i);
    for (int j = 0; j < sortLeaf->GetLen(); j++) {
      sort_value = sortLeaf->GetValue(j);     // in keV
      weight_value = weightLeaf->GetValue(j); // weight
      pos = posLeaf->GetValue(j);
      col = colLeaf->GetValue(j);
      csi = csiLeaf->GetValue(0); // recoil in csi only once per event
      group = group_map[pos][col][csi];

      /* printf("pos %2d col %1d E %.3f w %.3f csi %2d E
       * %.3f\n",pos,col,sort_value,weight_value,csi,csiE); */
      /* getc(stdin); */

      // drop bad hpge crystals and csi
      /**************************************************************
       for 84Kr: 10.2 (38), 11.0 (40), 11.2 (42), 12.2 (46), no csi
       for 94Sr: 12.2 (46), drop corner CsI (11,15,19,23)
      **************************************************************/
      int hpge = (pos - 1) * 4 + col; // 0-3 pos1, 4-7 pos2, etc.

      if (hpge != 38)
        if (hpge != 40)
          if (hpge != 42)
            if (hpge != 46)
            /* if(csi != 11) */
            /* if(csi != 15) */
            /* if(csi != 19) */
            /* if(csi != 23) */
            {
              if (sort_value >= 0.0) {
                if (fwhmResponse == false)
                  histVal = sort_value * sort_scaling + sort_shift;
                else {
                  histVal = FWHM_response(sort_value);
                  histVal = histVal * sort_scaling + sort_shift;
                }

                if (histVal >= 0.0)
                  if (histVal < S32K)
                    dOutHist[group][(int)(histVal)] +=
                        (float)weight_value; // fill the output histogram

                groupHist[0][group]++;
              }
            }
    }
  }
}

void readGroupMap() {
  FILE *inp;
  char line[132];
  int pos, col, csi, group;

  if ((inp = fopen(group_file, "r")) == NULL) {
    printf("\nI can't open file %s\n", group_file);
    exit(1);
  }
  /* printf("\nTIGRESS-CsI group map read from the file %s\n",group_file); */

  if (fgets(line, 132, inp) != NULL) {
    if (fgets(line, 132, inp) != NULL)
      while (fscanf(inp, "%d %d %d %d", &pos, &col, &csi, &group) != EOF)
        if (csi >= 1 && csi <= 24)
          if (pos >= 1 && pos <= 16)
            if (col >= 0 && col <= 3)
              group_map[pos][col][csi] = group;

  } else {
    printf("Wrong structure of file %s\n", group_file);
    printf("Aborting sort\n");
    exit(1);
  }
  fclose(inp);
}

double FWHM_response(double ch_in) {
  if (ch_in == 0.)
    return ch_in;

  double ch_out, fwhm, sigma, ch;
  double roll = randGen->Uniform();

  if ((wL > 0.0) || (wH > 0.0)) // gaussian with exponential tail(s)
  {
    if (roll < wG) // gaussian response
    {
      ch = ch_in / 1000.;
      fwhm = sqrt(fwhmF * fwhmF + fwhmG * fwhmG * ch + fwhmH * fwhmH * ch * ch);
      sigma = fwhm / 2.35482;
      if (sigma > 0)
        ch_out = randGen->Gaus(ch_in, sigma);
      else
        ch_out = ch_in;
    } else if (roll < (wG + wH)) // high energy exponential response
    {
      ch_out = ch_in + randGen->Exp(fwhmTauH);
    } else // low energy exponential response
    {
      ch_out = ch_in - randGen->Exp(fwhmTauL);
    }
  } else // gaussian only
  {
    ch = ch_in / 1000.;
    fwhm = sqrt(fwhmF * fwhmF + fwhmG * fwhmG * ch + fwhmH * fwhmH * ch * ch);
    sigma = fwhm / 2.35482;
    if (sigma > 0)
      ch_out = randGen->Gaus(ch_in, sigma);
    else
      ch_out = ch_in;
  }

  return ch_out;
}
