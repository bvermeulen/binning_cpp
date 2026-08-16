#include <iostream>
#include <fstream>
#include <cmath>
#include <iterator>
#include <boost/json/src.hpp>
#include "config.h"

using namespace std;

ConfigStruct read_config(string file_config) {
    string file_stem;
    float azimuth, offset, bin_sp_int, bin_rp_int;
    double origin_easting, origin_northing;
    int nb_bin_sp, nb_bin_rp, epsg;
    vector<int> src_indexes;
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
        origin_easting = boost::json::value_to<double>(obj.at("origin_easting"));
        origin_northing = boost::json::value_to<double>(obj.at("origin_northing"));
        bin_sp_int = boost::json::value_to<float>(obj.at("bin_sp_int"));
        bin_rp_int = boost::json::value_to<float>(obj.at("bin_rp_int"));
        nb_bin_sp = boost::json::value_to<int>(obj.at("nb_bin_sp"));
        nb_bin_rp = boost::json::value_to<int>(obj.at("nb_bin_rp"));
        offset = boost::json::value_to<float>(obj.at("offset"));
        src_indexes = boost::json::value_to<vector<int>>(obj.at("src_indexes"));
        epsg = boost::json::value_to<int>(obj.at("epsg"));
    }
    catch (exception const& e) {
        printf("Parsing failed: %s\n", e.what());
        exit(0);
    }

    ConfigStruct cfg;
    cfg.file_stem = file_stem;
    cfg.azimuth = azimuth;
    cfg.easting_orig = origin_easting;
    cfg.northing_orig = origin_northing;
    cfg.nb_bin_sp = nb_bin_sp;
    cfg.nb_bin_rp = nb_bin_rp;
    cfg.bin_sp_int = bin_sp_int;
    cfg.bin_rp_int = bin_rp_int;
    cfg.offset = offset;
    cfg.src_indexes = src_indexes;
    cfg.epsg = epsg;
    cfg.base_linepoint = BASE_LINEPOINT;
    return cfg;
}
