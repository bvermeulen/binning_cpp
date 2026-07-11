#ifndef BINNING_H
#define BINNING_H
#include <cmath>
#include "config.h"
#include "read_parse_sps.h"
#include "bins.h"
#include "binning.h"

using namespace std;

// avoid circular import as SaveData uses binning.h
class SaveData;

struct TraceStruct {
    int src_line;
    int src_point;
    int src_index;
    string src_code;
    int rcv_line;
    int rcv_point;
    int rcv_index;
    string rcv_code;
    double mid_point_x;
    double mid_point_y;
    float offset;
    float azimuth;
    int bin_sp;
    int bin_rp;
};

class Binning 
{
    public:
        Binning(
            const ConfigStruct& config,
            BinCalc& bincalc, 
            SaveData& savedata, 
            const vector<RcvStruct>& rcv, 
            const vector<SrcStruct>& src, 
            const vector<XStruct>& xrel
        );
        vector<TraceStruct> traces;
        void bin_sps();

    private:
        const ConfigStruct& cfg;
        BinCalc& bc;
        SaveData& sd;
        const vector<RcvStruct>& rcv_sps;
        const vector<SrcStruct>& src_sps;
        const vector<XStruct>& x_sps;
};

#endif // BINNING_H