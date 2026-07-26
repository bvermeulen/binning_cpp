#ifndef SAVE_DATA_H
#define SAVE_DATA_H
#include <sqlite3.h>
#include "config.h"
#include "traces.h"
#include "bins.h"

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
    
    public:
        DbHandling(const ConfigStruct& config);
        void create_database(string filename);
        void create_bins_table();
        void create_traces_table();
        void create_seis_config_table();
        void insert_bins(const vector<BinStruct>& bins);
        void insert_traces(const vector<TraceStruct>& traces);
        void index_traces();
        void update_seis_config(string key, string value);
        void store_config();
        void close_database();
};

#endif // SAVE_DATA_H