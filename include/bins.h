#ifndef BINS_H
#define BINS_H
#include "config.h"

using namespace std;

struct BinStruct {
    int bin_id;
    int bin_sp;
    int bin_rp;
    double easting;
    double northing;
    int bin_count;
    vector<int> src_indexes;
    int offset;
};

struct BinOffsetStruct {
    int bin_id;
    int offset;
    int src_index;
    int bin_count;
};

class BinCalc
{
    public:
        BinCalc(const ConfigStruct& config);
        vector<BinStruct> bins;
        vector<BinOffsetStruct> bins_offset;
        tuple<double, double> calc_bin_coordinate(double src_distance, double rcv_distance);
        tuple<int, int> calc_bin_grid(double x, double y);
        int calc_point_index(int i, int j);
        void create_bins();
        void create_bins_offset();

    private:
        const ConfigStruct& cfg;
        float cos_azim = cos(cfg.azimuth * DEG2RAD);
        float sin_azim = sin(cfg.azimuth * DEG2RAD);
        float cos_azim_ccw = cos(-cfg.azimuth * DEG2RAD);
        float sin_azim_ccw = sin(-cfg.azimuth * DEG2RAD);
        double sp_int = cfg.bin_sp_int;
        double rp_int = cfg.bin_rp_int;
        tuple<double, double> xy_rotation_clockwise(double x, double y);
        tuple<double, double> xy_rotation_ccw(double x, double y);
};

#endif  // BINS_H