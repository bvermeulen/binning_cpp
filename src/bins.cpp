#include <fstream>
#include <vector>
#include <string>
#include "config.h"
#include "bins.h"

using namespace std;

BinCalc::BinCalc(const ConfigStruct& config) : cfg(config) {}

tuple<double, double> BinCalc::xy_rotation_clockwise(double x, double y)
{
    double x_trans = x * cos_azim + y * sin_azim;
    double y_trans = -x * sin_azim + y * cos_azim;
    return make_tuple(x_trans, y_trans);
}

tuple<double, double> BinCalc::xy_rotation_ccw(double x, double y)
{
    double x_trans = x * cos_azim_ccw + y * sin_azim_ccw;
    double y_trans = -x * sin_azim_ccw + y * cos_azim_ccw;
    return make_tuple(x_trans, y_trans);
}

tuple<double, double> BinCalc::calc_bin_coordinate(double src_distance, double rcv_distance)
{
    auto coord = xy_rotation_clockwise(src_distance, rcv_distance);
    double x = get<0>(coord) + cfg.bin_easting_orig;
    double y = get<1>(coord) + cfg.bin_northing_orig;
    return make_tuple(x, y);
}

tuple<int, int> BinCalc::calc_bin_grid(double x, double y)
{
    x -= cfg.bin_easting_orig;
    y -= cfg.bin_northing_orig;
    auto coord = xy_rotation_ccw(x, y);
    int index_sp = round(get<0>(coord) / sp_int);
    int index_rp = round(get<1>(coord) / rp_int);
    return make_tuple(index_sp, index_rp);
}

int BinCalc::calc_point_index(int i, int j) {
    return (cfg.base_linepoint + i) * cfg.base_linepoint * 10 + (cfg.base_linepoint + j);
}

void BinCalc::create_bins() {
    for (int i=0; i <
        cfg.bin_nb_sp; i++) {
        double src_distance = i * cfg.bin_sp_int;
        for (int j=0; j < cfg.bin_nb_rp; j++) {
            double rcv_distance = j * cfg.bin_rp_int;
            auto coord = calc_bin_coordinate(src_distance, rcv_distance);
            BinStruct bin;
            bin.bin_id = calc_point_index(i, j);
            bin.bin_sp = i;
            bin.bin_rp = j;
            bin.easting = get<0>(coord);
            bin.northing = get<1>(coord);
            bin.bin_count = 0;
            bin.offset = 0;
            bin.src_indexes = vector<int> {};
            bins.push_back(bin);
        }
    }
    printf("number of bins: %'lu\n", bins.size());
}

void BinCalc::create_bins_offset() {
    for (int i = 0; i < cfg.bin_nb_sp; i++) {
        for (int j = 0; j < cfg.bin_nb_rp; j++) {
            int bin_id = calc_point_index(i, j);
            for (auto si : cfg.src_indexes) {
                for (int ofr = cfg.offset_range[0]; ofr <= cfg.offset_range[1]; ofr += cfg.offset_range[2]) {
                    BinOffsetStruct bin;
                    bin.bin_id = bin_id;
                    bin.offset = ofr;
                    bin.src_index = si;
                    bin.bin_count = 0;
                    bins_offset.push_back(bin);
                }
            }
        }
    }
    printf("number of bins offset: %'lu\n", bins_offset.size());
}
