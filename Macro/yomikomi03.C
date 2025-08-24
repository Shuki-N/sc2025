#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>

double analyze(TH1* h) {
    if (!h) {
        std::cerr << "Error: Invalid histogram passed to calculateValue." << std::endl;
        return 0;
    }

    std::vector<std::pair<double,double>> ranges = {
        {800,850}, {855,910}, {915,950}
    }; //暫定の各ピークの範囲(どこまでのピークを取るかに応じて要変更)

    double sum_photon_number = 0; //各データの平均光子数
    int photon_number = 0; //光子数(0光子からカウント)
    for (auto& r : ranges) { //光子数ごとの繰り返し
        //binの範囲
        int binMin = h->FindBin(r.first);
        int binMax = h->FindBin(r.second);
        
        //その範囲でイベント数を足し合わせる
        double sum = 0.0;
        for (int b = binMin; b <= binMax; ++b) {
            sum += h->GetBinContent(b);
        }
        //光子数で重みづけして和を取る
        sum_photon_number += sum * photon_number;
        photon_number++;
    }
    return sum_photon_number;
}


TGraph* yomikomi02(std::string filebase, int nfiles, float X0, float dx, int NumEvent){
    //filebase:ファイル名、nfiles: 読み込みファイル数、X0: グラフの原点、dx: プロットする時の刻み幅
    float x[1000], y[1000]; //グラフ用のデータを入れる配列
    int valid_points = 0; 
    for(int i=0; i<nfiles; i++){
        auto fn = Form("%s_%03d.root", filebase.c_str(), i); //ファイル名のみ読み取り
        auto file = TFile::Open(fn,"READ"); //ファイルの中身を代入
        if (!file || file->IsZombie()) {
            std::cout << "Warning: Skipping file " << fn << " (does not exist or is corrupted)." << std::endl;
            continue; // このファイルの処理を中断し、次のループに進む
        }

        auto h2 = dynamic_cast<TH1*>(file->Get("ADC_HIGH_10"));
        if (!h2) {
            std::cout << "Warning: Histogram not found in " << fn << ". Skipping." << std::endl;
            file->Close(); // ファイルは開いているので閉じる
            continue; // このファイルの処理を中断し、次のループに進む
        }

        x[valid_points] = X0 + dx*float(i);
        y[valid_points] = analyze(h2) /10000;
        valid_points++; // 正常に処理できた点の数を増やす

        file->Close();
       /* x[i] = X0 + dx*float(i);
        y[i] = analyze(h2)/NumEvent; //規格化も込みで値を出す */
    }

    TGraph* g = new TGraph(nfiles, x, y);
    g -> SetMarkerStyle(20);
    g -> GetXaxis() -> SetTitle("X [mm]");
    g -> GetYaxis() -> SetTitle("Average Number of Photons");
    g -> Draw("ALP");
    return g;
}
