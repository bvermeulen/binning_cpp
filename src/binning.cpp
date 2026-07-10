#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include "config.h"
#include "read_parse_sps.h"
#include "save_data.h"
#include "bins.h"
#include "binning.h"

using namespace std;

Binning::Binning(
    const ConfigStruct& config, 
    SaveData& savedata, 
    const vector<RcvStruct>& rcv, 
    const vector<SrcStruct>& src, 
    const vector<XStruct>& xrel,
    vector<BinStruct>& bins_ref
) : cfg(config), sd(savedata), rcv_sps(rcv), src_sps(src), x_sps(xrel), bins(bins_ref) {}

void Binning::bin_sps()
{
    int src_line, src_point, src_index;
    int rcv_line, rcv_point, rcv_point_start, rcv_point_end, rcv_index;
    string src_code, rcv_code;
    int id, src_bin, rcv_bin, trace_count;
    double src_easting, src_northing, mid_point_x, mid_point_y, dx, dy;
    float azimuth, offset;
    BinCalc bin(cfg);
    TraceStruct trace;
    vector<TraceStruct> traces;

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
        src_code = src_item -> p_code;
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
            if (trace_count % cfg.batch_size == 0) {
                if (trace_count != 0) {
                    printf("Trace count: %'11d: ", trace_count);
                    sd.insert_traces(traces);
                }
                traces.clear();
            }

            mid_point_x = (src_easting + rcv_item.easting) * 0.5;
            mid_point_y = (src_northing + rcv_item.northing) * 0.5;
            auto val = bin.calc_bin_index(mid_point_x, mid_point_y);
            src_bin = get<0>(val);
            rcv_bin = get<1>(val);
            if (src_bin > cfg.nb_bin_sp || rcv_bin > cfg.nb_bin_rp) continue;
            rcv_point = rcv_item.point;
            rcv_code = rcv_item.p_code;
            dx = rcv_item.easting - src_easting;
            dy = rcv_item.northing - src_northing;
            azimuth = atan2(dy, dx) * RAD_TO_DEG;
            offset = sqrt(dx*dx + dy*dy);
            trace.src_line = src_line;
            trace.src_point = src_point;
            trace.src_index = src_index;
            trace.src_code = src_code;
            trace.rcv_line = rcv_line;
            trace.rcv_point = rcv_point;
            trace.rcv_index = rcv_index;
            trace.rcv_code = rcv_code;
            trace.mid_point_x = mid_point_x;
            trace.mid_point_y = mid_point_y;
            trace.offset = offset;
            trace.azimuth = azimuth;
            trace.bin_sp = src_bin;
            trace.bin_rp = rcv_bin;
            traces.push_back(trace);
            trace_count++;

            id = bin.calc_point_index(src_bin, rcv_bin);
            auto bin_item = lower_bound(bins.begin(), bins.end(), id, 
                [](BinStruct& b, int value){return b.id < value;}
            );
            if (bin_item != bins.end() && offset <= cfg.max_offset) {
                bin_item->bin_count++;
            }
        }
    }
    printf("Trace count: %'11d: ", trace_count);
    sd.insert_traces(traces);
}
