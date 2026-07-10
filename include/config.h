#ifndef CONFIG_H
#define CONFIG_H
#include <cmath>

using namespace std;

const float DEG_TO_RAD = acos(-1.0) / 180.0;
const float RAD_TO_DEG = 180 / acos(-1.0);

struct ConfigStruct {
    string bin_files_stem;
    tuple<double, double, float> origin;
    int nb_bin_sp;
    int nb_bin_rp;
    double bin_sp_int;
    double bin_rp_int;
    float max_offset;
    int epsg;
    int base_linepoint;
    int batch_size;
};

ConfigStruct read_config(string file_config);

#endif // CONFIG_H
