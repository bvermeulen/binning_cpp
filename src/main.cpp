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
    string config_file;
    if (argc == 2) {
        config_file = argv[1];
    }
    else
    {
        config_file = "./config.json";
    }
    vector<RcvStruct> rcv_sps;
    vector<SrcStruct> src_sps;
    vector<XStruct> x_sps;
    ConfigStruct cfg = read_config(config_file);
    BinCalc bc(cfg);
    SaveData sd(cfg);
    Binning binning(cfg, bc, sd, rcv_sps, src_sps, x_sps);
    sd.create_database(cfg.bin_files_stem + ".sqlite");
    sd.create_bins_table();
    sd.create_traces_table();

    setlocale(LC_ALL, "");
    sps::parse_rcv_sps(cfg.bin_files_stem + ".R", rcv_sps);
    sps::parse_src_sps(cfg.bin_files_stem + ".S", src_sps);
    sps::parse_x_sps(cfg.bin_files_stem + ".X", x_sps);
    bc.create_bins();
    printf("number of bins: %'lu\n", bc.bins.size());
    binning.bin_sps();
    int i = 400;
    int j = 700;
    auto bin = ranges::find_if(bc.bins, [i, j](const BinStruct& b) {return (b.bin_sp == i && b.bin_rp == j);});
    printf("bin_sp: %d, bin_rp: %d, easting: %.0f, northing: %.0f, bin_count: %d\n",
        bin->bin_sp, bin->bin_rp, bin->easting, bin->northing, bin->bin_count
    );
    sd.save_bins_csv(cfg.bin_files_stem + ".csv", bc.bins);
    sd.insert_bins(bc.bins);
    sd.close_database();
}
