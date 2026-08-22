#ifndef DATA_HANDLING_H
#define DATA_HANDLING_H
#include <sqlite3.h>
#include "config.h"
#include "traces.h"
#include "bins.h"
#include "read_parse_sps.h"

using namespace std;

class CsvHandling
{
    private:
        const ConfigStruct& cfg;
    public:
        CsvHandling(const ConfigStruct& config);
        void save_bins_csv(string filename, const vector<BinStruct>& bins);
};


class DbHandling
{
    private:
        const ConfigStruct& cfg;
        sqlite3 *db;
        string vector_to_string(const vector<int>& intvector);

    public:
        DbHandling(const ConfigStruct& config);
        void create_database(string filename);
        void create_seis_config_table();
        void store_config();
        void update_seis_config(string key, string value);
        void create_bins_table();
        void insert_bins(const vector<BinStruct>& bins);
        void create_bins_offset_table();
        void insert_bins_offset(const vector<BinOffsetStruct>& bins_offset);
        void create_traces_table();
        void create_sps_rcv_table();
        void insert_sps_rcv(const vector<RcvStruct>& rcv_sps);
        void create_sps_src_table();
        void insert_sps_src(const vector<SrcStruct> &src_sps);
        void create_sps_x_table();
        void insert_sps_x(const vector<XStruct> &x_sps);
        void close_database();
};

#endif // DATA_HANDLING_H
