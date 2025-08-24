double analyze(TH1* h) {
    if (!h) {
        std::cerr << "Error: Invalid histogram passed to calculateValue." << std::endl;
        return 0;
    }

    std::vector<std::pair<double,double>> ranges = {
        {800,860}, {860,910}, {920,980}, {970,1030}, {1020,1080}
    }; //暫定の各ピークの範囲(どこまでのピークを取るかに応じて要変更)

    double sum_photon_number = 0; //各データの平均光子数
    int photon_number = 0; //光子数(0光子からカウント)
    for (auto& r : ranges) { //光子数ごとの繰り返し
        //binの範囲
        int binMin = h->FindBin(r.first);
        int binMax = h->FindBin(r.second);
        //このbinの範囲における最大値の探索
        double yMax = 0;
        int binCenter = binMin;
        for (int b = binMin; b <= binMax; ++b) {
            double val = h->GetBinContent(b);
            if (val > yMax) { yMax = val; binCenter = b; }
        }
        //最大値を基に各光子数のピークの範囲を精密に絞る(50%区間)
        double yCut = yMax * 0.2854;
        int binLeft = binCenter;
        while (binLeft > binMin && h->GetBinContent(binLeft) > yCut) {
             binLeft--; 
        }
        int binRight = binCenter;
        while (binRight < binMax && h->GetBinContent(binRight) > yCut) {
             binRight++;
        }
        //その範囲でイベント数を足し合わせる
        double sum = 0.0;
        for (int b = binLeft; b <= binRight; ++b) {
            sum += h->GetBinContent(b);
        }
        //光子数で重みづけして和を取る
        sum_photon_number += sum * photon_number;
        photon_number++;
    }
    return sum_photon_number;
}

TGraph* yomikomi02(std::string filebase, int nfiles, float X0, float dx){
    //filebase:ファイル名、nfiles: 読み込みファイル数、X0: グラフの原点、dx: プロットする時の刻み幅
    float x[1000], y[1000]; //グラフ用のデータを入れる配列
    for(int i=0; i<nfiles; i++){
        auto fn = Form("%s_%03d.root", filebase.c_str(), i); //ファイル名のみ読み取り
        auto file = TFile::Open(fn,"READ"); //ファイルの中身を代入
        auto h2 = dynamic_cast<TH1*>(file->Get("ADC_HIGH_10"));
        x[i] = X0 + dx*float(i);
        y[i] = analyze(h2);
    }
    TGraph* g = new TGraph(nfiles, x, y);
    g -> SetMarkerStyle(20);
    g -> SetXTitle("X [mm]");
    g -> SetYTitle("Average Number of Photons");
    g -> Draw("AP");
    return g;
}
