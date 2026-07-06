#ifndef SAVE_DATA_H
#define SAVE_DATA_H
#include <sqlite3.h>
#include "bins.h"

using namespace std;

class SaveData
{
    private:
        sqlite3 *db;
    
    public:
        void save_bins_csv(string filename, const vector<BinStruct>& bins);
        void create_database(string filename);
        void create_bin_table();
        void insert_bins(const vector<BinStruct>& bins);
        void close_database();
};

#endif // SAVE_DATA_H