#ifndef CONFIG_H
#define CONFIG_H
#include <cmath>

using namespace std;

const float DEG2RAD = acos(-1.0) / 180.0;
const float RAD2DEG = 180 / acos(-1.0);
const int BASE_LINEPOINT = 10'000;

struct ConfigStruct {
    string file_stem;
    float azimuth;
    double easting_orig;
    double northing_orig;
    int nb_bin_sp;
    int nb_bin_rp;
    double bin_sp_int;
    double bin_rp_int;
    float offset;
    vector<int> src_indexes;
    int epsg;
    int base_linepoint;
};

ConfigStruct read_config(string file_config);

#endif // CONFIG_H
