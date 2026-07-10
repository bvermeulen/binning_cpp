#include <vector>
#include <iostream>
#include <algorithm>
#include <clocale>
#include "config.h"
#include "read_parse_sps.h"
#include "bins.h"
#include "binning.h"
#include "save_data.h"

using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    string config_file;
    if (argc == 2) {
        config_file = argv[1];
    }
    else
    {
        config_file = "./config.json";
    }
    ConfigStruct cfg = read_config(config_file);
    vector<RcvStruct> rcv_sps, matched_rcv;
    vector<SrcStruct> src_sps;
    vector<XStruct> x_sps;
    vector<BinStruct> bins, matched_bins;
    BinCalc bc(cfg);
    SaveData sd(cfg);
    Binning binning(cfg, sd, rcv_sps, src_sps, x_sps, bins);
    
    sps::parse_rcv_sps(cfg.bin_files_stem + ".R", rcv_sps);
    sps::parse_src_sps(cfg.bin_files_stem + ".S", src_sps);
    sps::parse_x_sps(cfg.bin_files_stem + ".X", x_sps);
    sd.create_database(cfg.bin_files_stem + ".sqlite");
    sd.create_bins_table();
    sd.create_traces_table();
    bc.create_bins(bins);
    printf("number of bins: %'lld\n", bins.size());
    binning.bin_sps();
    sd.insert_bins(bins);
    int i = 400;
    int j = 700;
    auto bin = find_if(bins.begin(), bins.end(), [i, j](const BinStruct& b) {return (b.bin_sp == i && b.bin_rp == j);});
    printf("bin_sp: %d, bin_rp: %d, easting: %.0f, northing: %.0f, bin_count: %d\n",
        bin->bin_sp, bin->bin_rp, bin->easting, bin->northing, bin->bin_count
    );
    sd.save_bins_csv(cfg.bin_files_stem + ".csv", bins);
    sd.close_database();
}
