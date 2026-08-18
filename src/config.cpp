#include <iostream>
#include <fstream>
#include <cmath>
#include <iterator>
#include <boost/json/src.hpp>
#include "config.h"

using namespace std;

ConfigStruct read_config(string file_config) {
    string file_stem;
    float
        azimuth, offset, bin_sp_int, bin_rp_int,
        rl_int, rp_int, sl_int, sp_int;
    double
        bin_easting_orig, bin_northing_orig, rcv_easting_orig,
        rcv_northing_orig, src_easting_orig, src_northing_orig;
    int
        bin_nb_sp, bin_nb_rp,
        rcv_line_orig, rcv_point_orig, src_line_orig, src_point_orig,
        epsg, base_linepoint;
    vector<int> src_indexes;
    vector<int> offset_range;
    ifstream file(file_config);
    if (!file.is_open()) {
        printf("Error, could not open the file: %s!\n", file_config.c_str());
        exit(0);
    }
    ostringstream ss;
    ss << file.rdbuf();
    string json_str = ss.str();

    try {
        boost::json::value jv = boost::json::parse(json_str);
        boost::json::object const& obj = jv.as_object();

        file_stem = boost::json::value_to<string>(obj.at("file_stem"));
        azimuth = boost::json::value_to<float>(obj.at("azimuth"));
        bin_easting_orig = boost::json::value_to<double>(obj.at("bin_easting_orig"));
        bin_northing_orig = boost::json::value_to<double>(obj.at("bin_northing_orig"));
        bin_sp_int = boost::json::value_to<float>(obj.at("bin_sp_int"));
        bin_rp_int = boost::json::value_to<float>(obj.at("bin_rp_int"));
        bin_nb_sp = boost::json::value_to<int>(obj.at("bin_nb_sp"));
        bin_nb_rp = boost::json::value_to<int>(obj.at("bin_nb_rp"));

        rcv_easting_orig = boost::json::value_to<double>(obj.at("rcv_easting_orig"));
        rcv_northing_orig = boost::json::value_to<double>(obj.at("rcv_northing_orig"));
        rcv_line_orig = boost::json::value_to<int>(obj.at("rcv_line_orig"));
        rcv_point_orig = boost::json::value_to<int>(obj.at("rcv_point_orig"));
        rl_int = boost::json::value_to<float>(obj.at("rl_int"));
        rp_int = boost::json::value_to<float>(obj.at("rp_int"));
        src_easting_orig = boost::json::value_to<double>(obj.at("src_easting_orig"));
        src_northing_orig = boost::json::value_to<double>(obj.at("src_northing_orig"));
        src_line_orig = boost::json::value_to<int>(obj.at("src_line_orig"));
        src_point_orig = boost::json::value_to<int>(obj.at("src_point_orig"));
        sl_int = boost::json::value_to<float>(obj.at("sl_int"));
        sp_int = boost::json::value_to<float>(obj.at("sp_int"));

        offset = boost::json::value_to<float>(obj.at("offset"));
        offset_range = boost::json::value_to<vector<int>>(obj.at("offset_range"));
        src_indexes = boost::json::value_to<vector<int>>(obj.at("src_indexes"));
        epsg = boost::json::value_to<int>(obj.at("epsg"));
        base_linepoint = boost::json::value_to<int>(obj.at("base_linepoint"));
    }
    catch (exception const& e) {
        printf("Parsing failed: %s\n", e.what());
        exit(0);
    }

    ConfigStruct cfg;
    cfg.file_stem = file_stem;
    cfg.azimuth = azimuth;
    cfg.bin_easting_orig = bin_easting_orig;
    cfg.bin_northing_orig = bin_northing_orig;
    cfg.bin_nb_sp = bin_nb_sp;
    cfg.bin_nb_rp = bin_nb_rp;
    cfg.bin_sp_int = bin_sp_int;
    cfg.bin_rp_int = bin_rp_int;
    cfg.rcv_easting_orig = rcv_easting_orig;
    cfg.rcv_northing_orig = rcv_northing_orig;
    cfg.rcv_line_orig = rcv_line_orig;
    cfg.rcv_point_orig = rcv_point_orig;
    cfg.rl_int = rl_int;
    cfg.rp_int = rp_int;
    cfg.src_easting_orig = src_easting_orig;
    cfg.src_northing_orig = src_northing_orig;
    cfg.src_line_orig = src_line_orig;
    cfg.src_point_orig = src_point_orig;
    cfg.sl_int = sl_int;
    cfg.sp_int = sp_int;
    cfg.offset = offset;
    cfg.offset_range = offset_range;
    cfg.src_indexes = src_indexes;
    cfg.epsg = epsg;
    cfg.base_linepoint = base_linepoint;
    return cfg;
}
