TGraph* yomikomi(std::string filebase, int nfiles, float X0, float dx){
    float x[1000], y[1000];
    for(int i=0; i<nfiles; i++){
        auto fn = Form("%s_%03d.root", filebase.c_str(), i);
        auto file = TFile::Open(fn,"READ");
        auto h2 = dynamic_cast<TH1*>(file->Get("ADC_HIGH_10"));
        x[i] = X0 + dx*float(i);
        y[i] = h2->GetMean();
    }
    TGraph* g = new TGraph(nfiles, x, y);
    g -> SetMarkerStyle(20);
    g -> SetXTitle("X [mm]");
    g -> SetYTitle("Average Number of Photons");
    g -> Draw("AP");
    return g;
}
