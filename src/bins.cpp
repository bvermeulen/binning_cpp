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
    double x = get<0>(coord) + get<0>(cfg.origin);
    double y = get<1>(coord) + get<1>(cfg.origin);
    return make_tuple(x, y);
}

tuple<int, int> BinCalc::calc_bin_index(double x, double y)
{
    x -= get<0>(cfg.origin);
    y -= get<1>(cfg.origin);
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
        cfg.nb_bin_sp; i++) {
        double src_distance = i * cfg.bin_sp_int;
        for (int j=0; j < cfg.nb_bin_rp; j++) {
            double rcv_distance = j * cfg.bin_rp_int;
            auto coord = calc_bin_coordinate(src_distance, rcv_distance);
            BinStruct bin;
            bin.id = calc_point_index(i, j);
            bin.bin_sp = i;
            bin.bin_rp = j;
            bin.easting = get<0>(coord);
            bin.northing = get<1>(coord);
            bin.bin_count = 0;
            bins.push_back(bin);
        }
    }
}
