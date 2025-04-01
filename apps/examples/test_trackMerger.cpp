// example application
// test the track merger object as stand-alone app.

// std
#include<iostream>
#include<string>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

// us
#include "trackMerger.hh"
#include "Event.hh"
#include "CLI11.hpp"


int main(int argc, char** argv)
{
      // command line interface
  CLI::App app{"Test Track Merger"};
  std::string fname = "sampled.root";
  std::string outfname = "merged.root";

  app.add_option("-i,--input", fname, "<input file name> Default: sampled.root");
  app.add_option("-o,--output", outfname, "<output file name> Default: merged.root");

  CLI11_PARSE(app, argc, argv);

  // needed at construction time
  int nant = 2;
  
  // data source: read from ROOT file, store under key 'raw'
  TFile ff(fname.data(),"READ");
  TTreeReader re("sampled", &ff);

  // data sink: write to Root, take from key
  TFile* outfile = new TFile(outfname.data(), "RECREATE");
  TTree* tr = new TTree("merged","merged data");
  tr->SetDirectory(outfile);

  // set up the track merger object
  auto merge = trackMerger(re, tr, nant);

  // run
  merge.Loop();
  
  std::cout << "app finished. " << std::endl;
  tr->Write();
  outfile->Close(); // free TTree
  ff.Close();
}
