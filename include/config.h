#ifndef CONFIG_H
#define CONFIG_H
#include <cmath>

using namespace std;

const float DEG2RAD = acos(-1.0) / 180.0;
const float RAD2DEG = 180 / acos(-1.0);

struct ConfigStruct {
    string file_stem;
    float azimuth;
    double easting_orig;
    double northing_orig;
    int nb_bin_sp;
    int nb_bin_rp;
    float bin_sp_int;
    float bin_rp_int;
    double rcv_easting_orig;
    double rcv_northing_orig;
    int rcv_line_orig;
    int rcv_point_orig;
    float rl_int;
    float rp_int;
    double src_easting_orig;
    double src_northing_orig;
    int src_line_orig;
    int src_point_orig;
    float sl_int;
    float sp_int;
    float offset;
    vector<int> offset_range;
    vector<int> src_indexes;
    int epsg;
    int base_linepoint;
};

ConfigStruct read_config(string file_config);

#endif // CONFIG_H
