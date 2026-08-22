#ifndef TRACES_H
#define TRACES_H
#include <cmath>
#include "config.h"
#include "read_parse_sps.h"
#include "bins.h"

using namespace std;

// avoid circular import as DbHandling uses binning.h
class DbHandling;
class CsvHamdling;

class Binning
{
    public:
        Binning(
            const ConfigStruct &config,
            BinCalc &bincalc,
            DbHandling &db_handle,
            vector<BinStruct> &bins_vector,
            vector<BinOffsetStruct> &bo_vector,
            const vector<RcvStruct> &rcv,
            const vector<SrcStruct> &src,
            const vector<XStruct> &xrel
        );
        void bin_traces();

    private:
        const ConfigStruct& cfg;
        BinCalc& bc;
        DbHandling& db;
        vector<BinStruct>& bins;
        vector<BinOffsetStruct>& bins_offset;
        const vector<RcvStruct>& rcv_sps;
        const vector<SrcStruct>& src_sps;
        const vector<XStruct>& x_sps;
        void update_bin_offset(int bin_id, int src_index, float offset);
};

#endif // BINNING_H