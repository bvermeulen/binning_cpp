#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include "binning.h"
#include "bins.h"
#include "read_parse_sps.h"
#include "save_data.h"

using namespace std;

void binning(
    const vector<RcvStruct>& rcv_sps,
    const vector<SrcStruct>& src_sps,
    const vector<XStruct>& x_sps,
    vector<BinStruct>& bins
) {
    int src_line, src_point, src_index;
    int rcv_line, rcv_point_start, rcv_point_end, rcv_index;
    int id, src_bin, rcv_bin, trace_count;
    double src_easting, src_northing, mid_point_x, mid_point_y, dx, dy;
    float azimuth, offset;
    BinCalc bin;

    trace_count = 0;
    for (const auto& x_row : x_sps) {
        src_line = x_row.src_line;
        src_point = x_row.src_point;
        src_index = x_row.src_index;
        auto src_item = find_if(
            src_sps.begin(), src_sps.end(), 
            [src_line, src_point, src_index](const SrcStruct& s)
            {return (s.line == src_line) && (s.point == src_point) && (s.p_index == src_index);}
        );
        src_easting = src_item->easting;
        src_northing = src_item->northing;
        rcv_line = x_row.rcv_line;
        rcv_point_start = x_row.rcv_point_start;
        rcv_point_end = x_row.rcv_point_end;
        rcv_index = x_row.rcv_index;
        vector<RcvStruct> rcv_items;
        copy_if(rcv_sps.begin(), rcv_sps.end(), back_inserter(rcv_items), 
            [rcv_line, rcv_point_start, rcv_point_end, rcv_index]
            (const RcvStruct& r) {return 
                (r.line == rcv_line) && 
                (r.point >= rcv_point_start) && (r.point <= rcv_point_end) &&
                (r.p_index == rcv_index);
            }
        );
        for (const auto& rcv_item : rcv_items) {
            mid_point_x = (src_easting + rcv_item.easting) * 0.5;
            mid_point_y = (src_northing + rcv_item.northing) * 0.5;
            dx = rcv_item.easting - src_easting;
            dy = rcv_item.northing - src_northing;
            azimuth = atan2(dy, dx) * cfg::rad_to_deg;
            offset = sqrt(dx*dx + dy*dy);
            if (offset > cfg::max_offset) continue;
            auto val = bin.calc_bin_index(mid_point_x, mid_point_y);
            src_bin = get<0>(val);
            rcv_bin = get<1>(val);
            if (src_bin > cfg::nb_bin_sp || rcv_bin > cfg::nb_bin_rp) continue;
            id = bin.calc_point_index(src_bin, rcv_bin);
            auto bin_item = lower_bound(bins.begin(), bins.end(), id, 
                [](BinStruct& b, int value){return b.id < value;}
            );
            if (bin_item != bins.end()) {
                bin_item->bin_count++;
                if (trace_count % 1'000'000 == 0) {
                    printf("trace count: %d\n", trace_count);
                };
                trace_count++;
            }
        }
    }
}

int main() {
    vector<RcvStruct> rcv_sps, matched_rcv;
    vector<SrcStruct> src_sps;
    vector<XStruct> x_sps;
    vector<BinStruct> bins, matched_bins;
    BinCalc bc;
    SaveData sd;
    sps::parse_rcv_sps(cfg::file_stem + ".R", rcv_sps);
    sps::parse_src_sps(cfg::file_stem + ".S", src_sps);
    sps::parse_x_sps(cfg::file_stem + ".X", x_sps);
    bc.create_bins(bins);
    printf("number of bins: %lld\n", bins.size());
    binning(rcv_sps, src_sps, x_sps, bins);
    int i = 400;
    int j = 700;
    auto bin = find_if(bins.begin(), bins.end(), [i, j](const BinStruct& b) {return (b.bin_sp == i && b.bin_rp == j);});
    printf("bin_sp: %d, bin_rp: %d, easting: %.0f, northing: %.0f, bin_count: %d\n",
        bin->bin_sp, bin->bin_rp, bin->easting, bin->northing, bin->bin_count
    );
    sd.save_bins_csv(cfg::file_stem + ".csv", bins);
    sd.create_database(cfg::file_stem + ".sqlite");
    sd.create_bin_table();
    sd.insert_bins(bins);
    sd.close_database();
    return 0;
}
