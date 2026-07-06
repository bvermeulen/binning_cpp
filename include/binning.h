#ifndef BINNING_H
#define BINNING_H
#include <cmath>

using namespace std;

namespace cfg {
    inline string file_stem = "./data/sps_phase1_final";
    inline float deg_to_rad = acos(-1.0) / 180.0;
    inline float rad_to_deg = 180 / acos(-1.0);
    inline tuple<double, double, float> origin(
        701692.0, 3450142.50, 90.0 * deg_to_rad
    );
    inline int nb_bin_sp = 780;
    inline int nb_bin_rp = 1280;
    inline double bin_sp_int = -25.0;
    inline double bin_rp_int = 12.5;
    inline float max_offset = 1000.0;
    inline int epsg = 32638;
    inline int base_pnt = 10'000;
}

#endif // BINNING_H