#include <vector>
#include <iostream>
#include <algorithm>
#include <clocale>
#include "config.h"
#include "read_parse_sps.h"
#include "bins.h"
#include "binning.h"
#include "data_handling.h"

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
    DbHandling db(cfg);
    CsvHandling csv(cfg);
    Binning binning(cfg, bc, db, bc.bins, rcv_sps, src_sps, x_sps);

    db.create_database(cfg.file_stem + ".sqlite");
    db.create_seis_config_table();
    db.store_config();
    db.create_bins_table();

    setlocale(LC_ALL, "");
    sps::parse_rcv_sps(cfg.file_stem + ".R", rcv_sps);
    sps::parse_src_sps(cfg.file_stem + ".S", src_sps);
    sps::parse_x_sps(cfg.file_stem + ".X", x_sps);
    bc.create_bins();
    printf("number of bins: %'lu\n", bc.bins.size());
    binning.bin_traces();
    db.insert_bins(bc.bins);
    db.close_database();
}
