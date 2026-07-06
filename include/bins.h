#ifndef BINS_H
#define BINS_H
#include "binning.h"

using namespace std;

struct BinStruct {
    int id;
    int bin_sp;
    int bin_rp;
    double easting;
    double northing;
    int bin_count;
};

class BinCalc
{
    public:
        tuple<double, double> calc_bin_coordinate(double src_distance, double rcv_distance);
        tuple<int, int> calc_bin_index(double x, double y);
        int calc_point_index(int i, int j);
        void create_bins(vector<BinStruct>& bins);
        void save_bins_csv(string filename, const vector<BinStruct>& bins);
    
    private:
        float cos_azim = cos(get<2>(cfg::origin));
        float sin_azim = sin(get<2>(cfg::origin));
        float cos_azim_ccw = cos(-get<2>(cfg::origin));
        float sin_azim_ccw = sin(-get<2>(cfg::origin));
        double sp_int = cfg::bin_sp_int;
        double rp_int = cfg::bin_rp_int;
        tuple<double, double> xy_rotation_clockwise(double x, double y);
        tuple<double, double> xy_rotation_ccw(double x, double y);
};

#endif  // BINS_H