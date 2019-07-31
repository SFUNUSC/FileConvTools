#include "tree2doppler_2Dmap.h"
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

  FILE *list;
  TFile *inp;

  // initialize histograms
  for (int i = 0; i < 17; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 129; k++) {
        for (int l = 0; l < 129; l++) {
          dopplerFactor[i][j][k][l] = 0.; // initialize all elements to 0
          counter[i][j][k][l] = 0;
        }
        
      }

  d = new TH2D("DopplerFactor", "DopplerFactor", 64, 1, 64, 16384, 1, 16384);
  d1 = new TH1D("DopplerHist", "DopplerHist", 100, 0.940, 1.06);

  if (argc != 2) {
    printf("\ntree2mca_group parameter_file\n");
    printf("-----------------------\nSorts TIGRESS add-back spectra gated by "
           "TIGRESS-CsI Doppler shift groups defined via a group map.\n\n");
    exit(-1);
  }

  

  readConfigFile(argv[1]); // grab data from the parameter file

  // sort list of ROOT files
  if (listMode == true) {
    // read in tree list file
    if ((list = fopen(inp_filename, "r")) == NULL) {
      printf("ERROR: Cannot open the input list file %s!\n", inp_filename);
      exit(-1);
    }
    // scan the list file for ROOT files and put their data into the output
    // histogram
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
      if (use_weights) {
        if ((weightLeaf = stree->GetLeaf(weight_path)) == NULL)
          if ((weightBranch = stree->GetBranch(weight_path)) == NULL) {
            printf(
                "ERROR: Weight data path '%s' doesn't correspond to a branch "
                "or leaf in the tree!\n",
                weight_path);
            exit(-1);
          }
        if (weightLeaf == NULL)
          weightLeaf =
              (TLeaf *)weightBranch->GetListOfLeaves()
                  ->First(); // get the first leaf from the specified branch
      }
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

      addTreeDataToOutHist();
    }
    fclose(list);
  } else {
    // not list mode, single ROOT tree file
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
      sortLeaf = (TLeaf *)sortBranch->GetListOfLeaves()
                     ->First(); // get the first leaf from the specified branch

    // weight leaf
    if (use_weights) {
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
    }
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

    addTreeDataToOutHist();
  }

  writeGroupMap();

  return 0;
}

// This function extracts data from the tree after
// the tree addresses have been set and puts it in
// the output histogram.
void addTreeDataToOutHist() {
  Double_t df_value, weight_value;
  Int_t pos, col, csi1, csi2;

  for (int i = 0; i < stree->GetEntries(); i++) {
    stree->GetEntry(i);
    // first value only for first interaction which defines the decay
    df_value = sortLeaf->GetValue(0); // in keV
    if (use_weights)
      weight_value = weightLeaf->GetValue(0); // weight
    else
      weight_value = 1.;
    pos = posLeaf->GetValue(0);
    col = colLeaf->GetValue(0);
    csi1 = csiLeaf->GetValue(0);
    if(csiLeaf->GetNdata()>1){
      csi2 = csiLeaf->GetValue(1); // second csi hit
      dopplerFactor[pos][col][csi1][csi2] += weight_value * df_value; // weighted average
      counter[pos][col][csi1][csi2] += weight_value;
      d1->Fill(df_value, weight_value);
    }
    /* printf("pos %2d col %1d csi %2d df %.4f w
     * %.4f\n",pos,col,csi,df_value,weight_value); */
    /* getc(stdin); */
  }
}

void writeGroupMap() {
  FILE *group_map;
  FILE *df_map;
  int tig, csiInd, group;

  df_map = fopen("Doppler_shift_map.par", "w");
  fprintf(df_map,
          "This is a Doppler shift map for the TIGRESS and CsI array\n");
  fprintf(df_map, "pos   col    csi1    csi2    df\n");

  group_map = fopen("Doppler_shift_group_map.par", "w");
  fprintf(group_map,
          "This is a Doppler shift group map for the TIGRESS and CsI array\n");
  fprintf(group_map, "pos   col    csi1    csi2    group\n");

  for (int i = 1; i < 17; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 1; k < 129; k++) {
        for (int l = 1; l < 129; l++) {

          if (counter[i][j][k][l] > 0.) {
          // nan check on df
          if (dopplerFactor[i][j][k][l] != dopplerFactor[i][j][k][l]) {
            printf(
                "dopplerFactor[%d][%d][%d][%d] = %f with counter[%d][%d][%d][%d] %f\n",
                i, j, k, l, dopplerFactor[i][j][k][l], i, j, k, l, counter[i][j][k][l]);
            exit(EXIT_FAILURE);
          }
          dopplerFactor[i][j][k][l] /= counter[i][j][k][l]; // weighted average
          tig = (i - 1) * 4 + j;
          csiInd = (k - 1) * 128 + l;

          // tig [1,...,64] csi [1,...,24] due to root binning from -1 (!)
          d->SetBinContent(tig + 1, csiInd, dopplerFactor[i][j][k][l]);
          fprintf(df_map, "%2d     %d     %2d     %2d     %7.6f\n", i, j, k, l, 
                  dopplerFactor[i][j][k][l]);

          // make the groups
          /***********************
          For 94Sr:
          1           D  >  1.025
          2  1.012 <  D  <= 1.025
          3  0.986 <  D  <= 1.012
          4  0.975 <  D  <= 0.986
          5  0.975 <= D

          For 84Kr:
          1           D  >  1.023
          2  1.010 <  D  <= 1.023
          3  0.990 <  D  <= 1.010
          4  0.978 <  D  <= 0.990
          5  0.978 <= D

          For 20Mg:
          1           D  >  1.017
          2  1.005 <  D  <= 1.017
          3  0.993 <  D  <= 1.005
          4  0.982 <  D  <= 0.993
          5  0.982 <= D
           **********************/

          if (dopplerFactor[i][j][k][l] > dg1) {
            group = 1;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
          if (dopplerFactor[i][j][k][l] > dg2 && dopplerFactor[i][j][k][l] <= dg1) {
            group = 2;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
          if (dopplerFactor[i][j][k][l] > dg3 && dopplerFactor[i][j][k][l] <= dg2) {
            group = 3;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
          if (dopplerFactor[i][j][k][l] > dg4 && dopplerFactor[i][j][k][l] <= dg3) {
            group = 4;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
          if (dopplerFactor[i][j][k][l] > dg5 && dopplerFactor[i][j][k][l] <= dg4) {
            group = 5;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
          if (dopplerFactor[i][j][k][l] <= dg5) {
            group = 6;
            fprintf(group_map, "%2d     %d     %2d     %2d     %d\n", i, j, k, l, group);
          }
        }
        
        }
      }
  fclose(df_map);
  fclose(group_map);

  // write histogram to disk
  char title[132];
  sprintf(title, "DopplerFactor.root");
  TFile f(title, "recreate");
  d->GetXaxis()->SetTitle("Tigress ID");
  d->GetYaxis()->SetTitle("CsI ID");
  d->SetMaximum(1.06);
  d->SetMinimum(0.94);
  d->SetOption("COLZ");
  d->Write();
  d1->Write();
}
