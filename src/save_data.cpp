#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <sqlite3.h>
#include "bins.h"
#include "save_data.h"

using namespace std;

void SaveData::save_bins_csv(string filename, const vector<BinStruct>& bins) {
    string line;
    ofstream outfile(filename);
    line = "id, bin_sp, bin_rp, easting, northing, bin_count\n";
    outfile << line;
    for (BinStruct bin : bins) {
        string id = to_string(bin.id) + ", ";
        string sp = to_string(bin.bin_sp) + ", ";
        string rp = to_string(bin.bin_rp) + ", ";
        string e = to_string(bin.easting) + ", ";
        string n = to_string(bin.northing) + ", ";
        string bc = to_string(bin.bin_count) + "\n";
        line = id + sp + rp + e + n + bc;
        outfile << line;
    }
    outfile.close();
}

void SaveData::create_database(string filename) {
    char *error_message = 0;
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(0);
    }

    rc = sqlite3_enable_load_extension(db, 1);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(0);
    }

    rc = sqlite3_load_extension(db, "mod_spatialite", 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

    const char *sql = "SELECT InitSpatialMetadata(1);";
    rc = sqlite3_exec(db, sql, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("SpatiaLite tables initialized successfully!\n");
    }
}

void SaveData::create_bin_table() {
    char *error_message = 0;
    int rc = 0;
    string sql;
    sql = (
        "CREATE TABLE bins ("
        "id INTEGER PRIMARY KEY, "
        "bin_sp INTEGER, "
        "bin_rp INTEGER, "
        "easting DOUBLE PRECISION, "
        "northing DOUBLE PRECISION, "
        "bin_count INT "
        ");"
    );
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("Table bins successfully added!\n");
    }
    sql = format(
        "SELECT AddGeometryColumn(\"bins\", "
        "\"geom\", {}, \"POINT\", \"XY\");", cfg::epsg
    );
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("Geometry successfully added!\n");
    }
}

void SaveData::insert_bins(const vector<BinStruct>& bins) {
    string sql;
    sqlite3_stmt* stmt;
    sql = (
        "INSERT INTO bins ( "
        "bin_sp, bin_rp, easting, northing, bin_count, geom) "
        "VALUES (?, ?, ?, ?, ?, MakePoint(?, ?, ?) "
        ");"
    );
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    for (BinStruct bin : bins) {
        sqlite3_bind_int(stmt, 1, bin.bin_sp);
        sqlite3_bind_int(stmt, 2, bin.bin_rp);
        sqlite3_bind_double(stmt, 3, bin.easting);
        sqlite3_bind_double(stmt, 4, bin.northing);
        if (bin.bin_count > 0) {
            sqlite3_bind_int(stmt, 5, bin.bin_count);
        }
        else {
            sqlite3_bind_null(stmt, 5);
        }
        sqlite3_bind_double(stmt, 6, bin.easting);
        sqlite3_bind_double(stmt, 7, bin.northing);
        sqlite3_bind_int(stmt, 8, cfg::epsg);
    
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Insert failed: %s", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("Bins successfully inserted!\n");    
}

void SaveData::close_database() {
    sqlite3_close(db);
}
