#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <sqlite3.h>
#include "config.h"
#include "bins.h"
#include "binning.h"
#include "save_data.h"

using namespace std;

SaveData::SaveData(const ConfigStruct& config) : cfg(config) {}

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
    if (sqlite3_open(filename.c_str(), &db)) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(0);
    }

    if (sqlite3_enable_load_extension(db, 1) != SQLITE_OK) {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(0);
    }

    if (sqlite3_load_extension(db, "mod_spatialite", 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

    const string sql = "SELECT InitSpatialMetadata(1);";
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("SpatiaLite tables initialized successfully!\n");
    }
}

void SaveData::create_bins_table() {
    char *error_message = 0;
    string sql;
    sql = (
        "DROP TABLE IF EXISTS bins;"
    );
        if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

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
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("Table bins successfully added!\n");
    }

    sql = format(
        "SELECT AddGeometryColumn(\"bins\", "
        "\"geom\", {}, \"POINT\", \"XY\");", cfg.epsg
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("Geometry successfully added!\n");
    }
}

void SaveData::create_traces_table() {
    char *error_message = 0;
    string sql;
    sql = (
        "DROP TABLE IF EXISTS traces;"
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    
    sql = (
        "CREATE TABLE traces ("
        "id INTEGER PRIMARY KEY, "
        "src_line INTEGER, "
        "src_point INTEGER, "
        "src_index INTEGER, "
        "src_code VAR(2), "
        "rcv_line INTEGER, "
        "rcv_point INTEGER, "
        "rcv_index INTEGER, "
        "rcv_code VAR(2), "
        "mid_point_x DOUBLE PRECISION, "
        "mid_point_y DOUBLE PRECISION, "
        "offset REAL, "
        "azimuth REAL, "
        "bin_sp INTEGER, "
        "bin_rp INTEGER "
        ");"
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK) {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    } else {
        printf("Table tracess successfully added!\n");
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
    for (const auto& bin : bins) {
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
        sqlite3_bind_int(stmt, 8, cfg.epsg);
    
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Insert failed: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("Bins successfully inserted: %'lld\n", bins.size());    
}

void SaveData::insert_traces(const vector<TraceStruct>& traces) {
    string sql;
    sqlite3_stmt* stmt;
    sql = (
        "INSERT INTO traces ( "
        "src_line, src_point, src_index, src_code, "
        "rcv_line, rcv_point, rcv_index, rcv_code, "
        "mid_point_x, mid_point_y, offset, azimuth, bin_sp, bin_rp) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?); "
    );
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    for (const auto& trace : traces) {
        sqlite3_bind_int(stmt, 1, trace.src_line);
        sqlite3_bind_int(stmt, 2, trace.src_point);
        sqlite3_bind_int(stmt, 3, trace.src_index);
        sqlite3_bind_text(stmt, 4, trace.src_code.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, trace.rcv_line);
        sqlite3_bind_int(stmt, 6, trace.rcv_point);
        sqlite3_bind_int(stmt, 7, trace.rcv_index);
        sqlite3_bind_text(stmt, 8, trace.rcv_code.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 9, trace.mid_point_x);
        sqlite3_bind_double(stmt, 10, trace.mid_point_y);
        sqlite3_bind_double(stmt, 11, trace.offset);
        sqlite3_bind_double(stmt, 12, trace.azimuth);
        sqlite3_bind_int(stmt, 13, trace.bin_sp);
        sqlite3_bind_int(stmt, 14, trace.bin_rp);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Insert failed: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("%'9lld traces successfully inserted\n", traces.size());    
}

void SaveData::close_database() {
    sqlite3_close(db);
}
