#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <sqlite3.h>
#include "config.h"
#include "bins.h"
#include "binning.h"
#include "data_handling.h"

using namespace std;

CsvHandling::CsvHandling(const ConfigStruct& config) : cfg(config) {}
DbHandling::DbHandling(const ConfigStruct& config) : cfg(config) {}

void CsvHandling::save_bins_csv(string filename, const vector<BinStruct>& bins) {
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

void DbHandling::create_database(string filename) {
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

void DbHandling::create_seis_config_table()
{
    char *error_message = 0;
    string sql;
    sql = ("DROP TABLE IF EXISTS seis_config;");
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    sql = ("CREATE TABLE seis_config ("
           "key TEXT PRIMARY KEY, "
           "value TEXT NOT NULL"
           ");");
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    else
    {
        printf("Table seis_config successfully added!\n");
    }
}

void DbHandling::update_seis_config(string key, string value)
{
    string sql;
    sqlite3_stmt *stmt;
    sql = ("INSERT OR REPLACE INTO seis_config (key, value) "
           "VALUES (?, ?);");
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        printf("Insert failed: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}

void DbHandling::store_config()
{
    update_seis_config("file_stem", cfg.file_stem);
    update_seis_config("azimuth", to_string(cfg.azimuth));
    update_seis_config("easting_orig", to_string(cfg.easting_orig));
    update_seis_config("northing_orig", to_string(cfg.northing_orig));
    update_seis_config("northing_orig", to_string(cfg.northing_orig));
    update_seis_config("bin_sp_int", to_string(cfg.bin_sp_int));
    update_seis_config("bin_rp_int", to_string(cfg.bin_rp_int));
    update_seis_config("nb_bin_sp", to_string(cfg.nb_bin_sp));
    update_seis_config("nb_bin_rp", to_string(cfg.nb_bin_rp));
    update_seis_config("epsg", to_string(cfg.epsg));
    update_seis_config("offset", to_string(cfg.offset));
    update_seis_config("src_indexes", "0");
    printf("config values stored in database!\n");
}

void DbHandling::create_bins_table() {
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

void DbHandling::insert_bins(const vector<BinStruct>& bins) {
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
    printf("Bins successfully inserted: %'lu\n", bins.size());
}

void DbHandling::create_sps_rcv_table()
{
    char *error_message = 0;
    string sql;
    sql = ("DROP TABLE IF EXISTS sps_rcv;");
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

    sql = (
        "CREATE TABLE sps_rcv ("
        "id INTEGER PRIMARY KEY, "
        "type VAR(1), "
        "line INTEGER, "
        "point INTEGER, "
        "p_index INTEGER, "
        "p_code VAR(2), "
        "easting DOUBLE PRECISION, "
        "northing DOUBLE PRECISION, "
        "elevation REAL"
        ");"
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    else
    {
        printf("Table sps_rcv successfully added!\n");
    }
}

void DbHandling::insert_sps_rcv(const vector<RcvStruct> &sps_rcv)
{
    string sql;
    sqlite3_stmt *stmt;
    sql = (
        "INSERT INTO sps_rcv ( "
        "type, line, point, p_index, p_code, easting, northing, elevation) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
    );
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    for (const auto &rcv : sps_rcv)
    {
        sqlite3_bind_text(stmt, 1, rcv.type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, rcv.line);
        sqlite3_bind_int(stmt, 3, rcv.point);
        sqlite3_bind_int(stmt, 4, rcv.p_index);
        sqlite3_bind_text(stmt, 5, rcv.p_code.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 6, rcv.easting);
        sqlite3_bind_double(stmt, 7, rcv.northing);
        sqlite3_bind_double(stmt, 8, rcv.elevation);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            printf("Insert failed: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("Receiver SPS successfully inserted: %'lu\n", sps_rcv.size());
}

void DbHandling::create_sps_src_table()
{
    char *error_message = 0;
    string sql;
    sql = ("DROP TABLE IF EXISTS sps_src;");
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

    sql = (
        "CREATE TABLE sps_src ("
        "id INTEGER PRIMARY KEY, "
        "type VAR(1), "
        "line INTEGER, "
        "point INTEGER, "
        "p_index INTEGER, "
        "p_code VAR(2), "
        "easting DOUBLE PRECISION, "
        "northing DOUBLE PRECISION, "
        "elevation REAL"
        ");"
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    else
    {
        printf("Table sps_src successfully added!\n");
    }
}

void DbHandling::insert_sps_src(const vector<SrcStruct> &sps_src)
{
    string sql;
    sqlite3_stmt *stmt;
    sql = (
        "INSERT INTO sps_src ( "
        "type, line, point, p_index, p_code, easting, northing, elevation) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
    );
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    for (const auto &src : sps_src)
    {
        sqlite3_bind_text(stmt, 1, src.type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, src.line);
        sqlite3_bind_int(stmt, 3, src.point);
        sqlite3_bind_int(stmt, 4, src.p_index);
        sqlite3_bind_text(stmt, 5, src.p_code.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 6, src.easting);
        sqlite3_bind_double(stmt, 7, src.northing);
        sqlite3_bind_double(stmt, 8, src.elevation);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            printf("Insert failed: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("Receiver SPS successfully inserted: %'lu\n", sps_src.size());
}

void DbHandling::create_sps_x_table()
{
    char *error_message = 0;
    string sql;
    sql = ("DROP TABLE IF EXISTS sps_x;");
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }

    sql = (
        "CREATE TABLE sps_x ("
        "id INTEGER PRIMARY KEY, "
        "type VAR(1), "
        "src_line INTEGER, "
        "src_point INTEGER, "
        "src_index INTEGER, "
        "chan_start INTEGER, "
        "chan_end INTEGER, "
        "rcv_line INTEGER, "
        "rcv_point_start INTEGER, "
        "rcv_point_end INTEGER, "
        "rcv_index INTEGER, "
        "tb var(50)"
        ");"
    );
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &error_message) != SQLITE_OK)
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        exit(0);
    }
    else
    {
        printf("Table sps_x successfully added!\n");
    }
}

void DbHandling::insert_sps_x(const vector<XStruct> &sps_x)
{
    string sql;
    sqlite3_stmt *stmt;
    sql = (
        "INSERT INTO sps_x ( "
        "type, src_line, src_point, src_index, chan_start, chan_end, "
        "rcv_line, rcv_point_start, rcv_point_enc, rcv_index, tb_var) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    for (const auto &x : sps_x)
    {
        sqlite3_bind_text(stmt, 1, x.type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, x.src_line);
        sqlite3_bind_int(stmt, 3, x.src_point);
        sqlite3_bind_int(stmt, 4, x.chan_start);
        sqlite3_bind_int(stmt, 5, x.chan_end);
        sqlite3_bind_int(stmt, 6, x.rcv_line);
        sqlite3_bind_int(stmt, 7, x.rcv_point_start);
        sqlite3_bind_int(stmt, 8, x.rcv_point_end);
        sqlite3_bind_int(stmt, 9, x.rcv_index);
        sqlite3_bind_int(stmt, 10, x.src_index);
        sqlite3_bind_text(stmt, 11, x.tb.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            printf("Insert failed: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    printf("X SPS successfully inserted: %'lu\n", sps_x.size());
}

void DbHandling::close_database() {
    sqlite3_close(db);
}
