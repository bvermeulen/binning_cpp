#ifndef SAVE_DATA_H
#define SAVE_DATA_H
#include <sqlite3.h>
#include "config.h"
#include "binning.h"
#include "bins.h"

using namespace std;

class SaveData
{
    private:
        const ConfigStruct& cfg;
        sqlite3 *db;
    
    public:
        SaveData(const ConfigStruct& config);
        void save_bins_csv(string filename, const vector<BinStruct>& bins);
        void create_database(string filename);
        void create_bins_table();
        void create_traces_table();
        void insert_bins(const vector<BinStruct>& bins);
        void insert_traces(const vector<TraceStruct>& traces);
        void close_database();
};

#endif // SAVE_DATA_H